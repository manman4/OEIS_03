/*
 * A318790 -- exact sparse subset DP with a variable-length frontier.
 *
 * Let d be the sequence index, N=d*d+1, and w=d-1.  We count permutations
 * p of [N] for which
 *
 *   |p(i+j)-p(i)| >= d       (1 <= j <= w),
 *
 * and divide by two (a permutation and its reversal form a pair).
 *
 * Once a valid prefix has been constructed, a new value has to be checked
 * only against its last w values.  The exact DP state is therefore
 *
 *   (used values, ordered last-w values).
 *
 * Only reachable states are stored.  Value complementation x -> N-1-x is an
 * automorphism; the default DP stores the two members of each orbit under one
 * canonical key.  --no-symmetry disables this quotient for verification.
 * A bounded look-ahead removes a state only when it provably has no extension
 * of the tested length.  A complete continuation would contain every shorter
 * extension, so this pruning cannot alter the answer.
 *
 * Safety and limits:
 *   - N<=50 (d<=7), so the used set and every value fit the packed state;
 *   - coefficients use checked unsigned 128-bit arithmetic: an overflow is
 *     reported, never silently wrapped;
 *   - hash growth, allocation sizes, and the user memory budget are checked;
 *   - a partial b-file is renamed only after the whole requested range ends.
 *
 * Verification:
 *   - OEIS terms a(1)..a(6) are built in;
 *   - --check compares quotient and non-quotient DP through d=4;
 *   - --check also compares an independent prefix backtracking count through
 *     d=3.
 *
 * Reference: https://oeis.org/A318790
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     318790_01.c -o 318790_01
 *
 * Usage:
 *   ./318790_01                  # write a(1)..a(6)
 *   ./318790_01 --upto 6
 *   ./318790_01 --term 6 --memory-mib 2048
 *   ./318790_01 --check
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#if !defined(__SIZEOF_INT128__)
#error "318790_01 requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 uint128_t;

#define OFFSET 1
#define DEFAULT_MAX_D 6
#define MAX_D 7
#define KNOWN_MAX_D 6
#define DEFAULT_MEMORY_MIB UINT64_C(1024)
#define MIN_MEMORY_MIB UINT64_C(16)
#define INITIAL_CAPACITY ((size_t)16)
#define LOAD_NUM ((size_t)7)
#define LOAD_DEN ((size_t)8)
#define VALUE_BITS 6U
#define VALUE_MASK UINT64_C(63)
#define LOOKAHEAD_MULTIPLIER 2

static const char *const known[KNOWN_MAX_D + 1] = {
    NULL, "1", "7", "20", "37", "64", "109"
};

typedef struct { uint64_t low, high; } Count;
typedef struct {
    uint64_t used;       /* zero denotes an empty hash-table slot */
    uint64_t frontier;   /* oldest value in the least significant field */
    uint64_t low, high;
} Slot;
typedef struct { size_t limit, in_use, peak; } Budget;
typedef struct {
    Slot *slots;
    size_t capacity, size;
    Budget *budget;
} Map;
typedef struct {
    uint64_t transitions, pruned;
    size_t peak_states, peak_bytes;
    double seconds;
} Stats;
typedef enum { MODE_UPTO, MODE_TERM, MODE_CHECK } Mode;

static void die(const char *s) { fprintf(stderr, "error: %s\n", s); exit(1); }

static double now_seconds(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) != 0) die("clock_gettime failed");
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static size_t checked_mul(size_t a, size_t b)
{
    if (a != 0 && b > SIZE_MAX / a) die("allocation size overflow");
    return a * b;
}

static int parse_d(const char *s, const char *label)
{
    char *end = NULL;
    errno = 0;
    long x = strtol(s, &end, 10);
    if (errno || end == s || *end || x < OFFSET || x > MAX_D) {
        fprintf(stderr, "error: %s must be in %d..%d: %s\n",
                label, OFFSET, MAX_D, s);
        exit(1);
    }
    return (int)x;
}

static uint64_t parse_memory(const char *s)
{
    char *end = NULL;
    errno = 0;
    unsigned long long x = strtoull(s, &end, 10);
    uint64_t max = (uint64_t)(SIZE_MAX / 1048576U);
    if (errno || end == s || *end || x < MIN_MEMORY_MIB || x > max) {
        fprintf(stderr, "error: MEMORY_MIB must be in %" PRIu64
                "..%" PRIu64 ": %s\n", MIN_MEMORY_MIB, max, s);
        exit(1);
    }
    return (uint64_t)x;
}

static Count count_u64(uint64_t x) { Count c = {x, 0}; return c; }

static void count_add(Count *a, Count b)
{
    uint64_t old = a->low;
    a->low += b.low;
    uint64_t carry = a->low < old;
    uint64_t h = a->high + b.high;
    if (h < a->high || h + carry < h) die("128-bit coefficient overflow");
    a->high = h + carry;
}

static bool count_equal(Count a, Count b)
{ return a.low == b.low && a.high == b.high; }

static Count count_half(Count a)
{
    if (a.low & 1U) die("oriented count is unexpectedly odd");
    Count b = {(a.low >> 1U) | (a.high << 63U), a.high >> 1U};
    return b;
}

static Count parse_count(const char *s)
{
    uint128_t x = 0, max = ~(uint128_t)0;
    for (; *s; ++s) {
        if (*s < '0' || *s > '9') die("invalid built-in term");
        unsigned d = (unsigned)(*s - '0');
        if (x > (max - d) / 10U) die("built-in term overflow");
        x = x * 10U + d;
    }
    Count c = {(uint64_t)x, (uint64_t)(x >> 64U)};
    return c;
}

static int print_count(FILE *f, Count a)
{
    char buf[40]; size_t n = 0;
    uint128_t x = ((uint128_t)a.high << 64U) | a.low;
    do { buf[n++] = (char)('0' + (unsigned)(x % 10U)); x /= 10U; } while (x);
    while (n) if (fputc(buf[--n], f) == EOF) return -1;
    return 0;
}

static void reserve(Budget *b, size_t bytes)
{
    if (bytes > b->limit - b->in_use) {
        fprintf(stderr, "error: DP memory budget exceeded: need %.1f MiB "
                "with %.1f/%.1f MiB in use\n", (double)bytes/1048576.0,
                (double)b->in_use/1048576.0, (double)b->limit/1048576.0);
        exit(1);
    }
    b->in_use += bytes;
    if (b->peak < b->in_use) b->peak = b->in_use;
}

static Slot *alloc_slots(Budget *b, size_t cap)
{
    size_t bytes = checked_mul(cap, sizeof(Slot));
    reserve(b, bytes);
    Slot *p = calloc(cap, sizeof(*p));
    if (!p) die("DP allocation failed");
    return p;
}

static void release_slots(Budget *b, Slot *p, size_t cap)
{
    free(p);
    size_t bytes = checked_mul(cap, sizeof(Slot));
    if (bytes > b->in_use) die("memory budget underflow");
    b->in_use -= bytes;
}

static uint64_t mix64(uint64_t x)
{
    x ^= x >> 30U; x *= UINT64_C(0xbf58476d1ce4e5b9);
    x ^= x >> 27U; x *= UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31U);
}

static size_t key_index(uint64_t used, uint64_t frontier, size_t mask)
{ return (size_t)mix64(used ^ mix64(frontier)) & mask; }

static Slot *map_find(Map *m, uint64_t used, uint64_t frontier)
{
    size_t i = key_index(used, frontier, m->capacity - 1U);
    for (;;) {
        Slot *s = &m->slots[i];
        if (s->used == 0 || (s->used == used && s->frontier == frontier))
            return s;
        i = (i + 1U) & (m->capacity - 1U);
    }
}

static void map_init_cap(Map *m, Budget *b, size_t cap)
{
    if (cap < INITIAL_CAPACITY || (cap & (cap - 1U)))
        die("invalid hash capacity");
    m->budget = b; m->capacity = cap; m->size = 0;
    m->slots = alloc_slots(b, cap);
}

static void map_init(Map *m, Budget *b) { map_init_cap(m, b, INITIAL_CAPACITY); }

static void map_destroy(Map *m)
{
    if (m->slots) release_slots(m->budget, m->slots, m->capacity);
    m->slots = NULL; m->capacity = m->size = 0;
}

static void map_grow(Map *m)
{
    if (m->capacity > SIZE_MAX/2U) die("hash capacity overflow");
    size_t oldcap = m->capacity;
    Slot *old = m->slots;
    m->capacity *= 2U;
    m->slots = alloc_slots(m->budget, m->capacity);
    for (size_t i=0; i<oldcap; ++i) if (old[i].used) {
        Slot *s = map_find(m, old[i].used, old[i].frontier);
        *s = old[i];
    }
    release_slots(m->budget, old, oldcap);
}

static void map_add(Map *m, uint64_t used, uint64_t frontier, Count add)
{
    Slot *s = map_find(m, used, frontier);
    if (s->used) {
        Count c = {s->low, s->high}; count_add(&c, add);
        s->low = c.low; s->high = c.high; return;
    }
    if (m->size + 1U > (m->capacity/LOAD_DEN)*LOAD_NUM) {
        map_grow(m); s = map_find(m, used, frontier);
    }
    s->used=used; s->frontier=frontier; s->low=add.low; s->high=add.high;
    ++m->size;
}

static void map_clear(Map *m)
{ memset(m->slots, 0, checked_mul(m->capacity,sizeof(Slot))); m->size=0; }

static size_t capacity_for(size_t entries)
{
    size_t c=INITIAL_CAPACITY;
    while ((c/LOAD_DEN)*LOAD_NUM < entries) {
        if (c>SIZE_MAX/2U) die("predicted capacity overflow"); c*=2U;
    }
    return c;
}

static void prepare_next(Map *m, size_t cap)
{
    if (cap<INITIAL_CAPACITY) cap=INITIAL_CAPACITY;
    if (m->capacity==cap) { map_clear(m); return; }
    Budget *b=m->budget; map_destroy(m); map_init_cap(m,b,cap);
}

static uint64_t reverse_bits(uint64_t x)
{
    x=((x&UINT64_C(0x5555555555555555))<<1)|((x>>1)&UINT64_C(0x5555555555555555));
    x=((x&UINT64_C(0x3333333333333333))<<2)|((x>>2)&UINT64_C(0x3333333333333333));
    x=((x&UINT64_C(0x0f0f0f0f0f0f0f0f))<<4)|((x>>4)&UINT64_C(0x0f0f0f0f0f0f0f0f));
    x=((x&UINT64_C(0x00ff00ff00ff00ff))<<8)|((x>>8)&UINT64_C(0x00ff00ff00ff00ff));
    x=((x&UINT64_C(0x0000ffff0000ffff))<<16)|((x>>16)&UINT64_C(0x0000ffff0000ffff));
    return (x<<32)|(x>>32);
}

static uint64_t reverse_low(uint64_t x, int bits)
{ return reverse_bits(x) >> (64U-(unsigned)bits); }

static uint64_t complement_frontier(uint64_t f, int width, int total)
{
    uint64_t r=0;
    for (int i=0;i<width;++i) {
        unsigned v=(unsigned)((f>>(VALUE_BITS*(unsigned)i))&VALUE_MASK);
        r |= (uint64_t)(total-1-(int)v) << (VALUE_BITS*(unsigned)i);
    }
    return r;
}

static void canonicalize(uint64_t *used, uint64_t *frontier,
                         int width, int total, bool symmetry)
{
    if (!symmetry) return;
    uint64_t cu=reverse_low(*used,total);
    uint64_t cf=complement_frontier(*frontier,width,total);
    if (cu < *used || (cu == *used && cf < *frontier)) {
        *used=cu; *frontier=cf;
    }
}

static uint64_t forbidden_for_frontier(uint64_t f, int width,
                                       const uint64_t *bad)
{
    uint64_t r=0;
    for (int i=0;i<width;++i) {
        unsigned v=(unsigned)((f>>(VALUE_BITS*(unsigned)i))&VALUE_MASK);
        r |= bad[v];
    }
    return r;
}

static uint64_t append_frontier(uint64_t f, int width, unsigned v)
{
    if (width==1) return v;
    return (f>>VALUE_BITS) |
           ((uint64_t)v << (VALUE_BITS*(unsigned)(width-1)));
}

static bool has_extension(uint64_t remaining, uint64_t frontier, int width,
                          const uint64_t *bad, int depth)
{
    if (!depth) return true;
    uint64_t candidates=remaining & ~forbidden_for_frontier(frontier,width,bad);
    while (candidates) {
        uint64_t bit=candidates & (UINT64_C(0)-candidates); candidates^=bit;
        unsigned v=(unsigned)__builtin_ctzll(bit);
        if (has_extension(remaining^bit,append_frontier(frontier,width,v),
                          width,bad,depth-1)) return true;
    }
    return false;
}

static void prune(Map *m, uint64_t full, int width, int index,
                  const uint64_t *bad, Stats *stats)
{
    for (size_t i=0;i<m->capacity;++i) {
        Slot *s=&m->slots[i]; if (!s->used) continue;
        uint64_t rem=full & ~s->used;
        int left=__builtin_popcountll(rem);
        /* Here N=index^2+1 is only just large enough to hold index widely
         * separated values in every window.  Consequently the branching
         * factor after a window has formed is small.  Looking through two
         * complete window lengths discards many globally impossible prefixes
         * before they consume a hash-table slot in the following layer. */
        int horizon=LOOKAHEAD_MULTIPLIER*index;
        int depth=left<horizon?left:horizon;
        if (!has_extension(rem,s->frontier,width,bad,depth)) {
            /* Clearing a slot would normally break a linear-probing search
             * chain.  A pruned map is never searched by key again: it is
             * only scanned sequentially as the current layer, and is fully
             * cleared or destroyed before it is reused as the next layer. */
            s->used=0; s->frontier=s->low=s->high=0; --m->size;
            if (stats->pruned==UINT64_MAX) die("prune counter overflow");
            ++stats->pruned;
        }
    }
}

static void update_stats(Stats *s, const Map *m, const Budget *b)
{
    if (s->peak_states<m->size) s->peak_states=m->size;
    if (s->peak_bytes<b->peak) s->peak_bytes=b->peak;
}

typedef struct {
    Map *map; const uint64_t *bad; int total,width;
    bool symmetry;
} InitContext;

static void initialize_rec(InitContext *c, int length, uint64_t used,
                           uint64_t frontier)
{
    if (length==c->width) {
        canonicalize(&used,&frontier,c->width,c->total,c->symmetry);
        map_add(c->map,used,frontier,count_u64(1)); return;
    }
    uint64_t full=(UINT64_C(1)<<(unsigned)c->total)-1U;
    uint64_t candidates=full & ~used;
    if (length) candidates &= ~forbidden_for_frontier(frontier,length,c->bad);
    while (candidates) {
        uint64_t bit=candidates & (UINT64_C(0)-candidates); candidates^=bit;
        unsigned v=(unsigned)__builtin_ctzll(bit);
        uint64_t nf=frontier | ((uint64_t)v << (VALUE_BITS*(unsigned)length));
        initialize_rec(c,length+1,used|bit,nf);
    }
}

static Count compute_oriented(int d, bool symmetry, uint64_t memory_mib,
                              Stats *stats)
{
    double started=now_seconds(); memset(stats,0,sizeof(*stats));
    int total=d*d+1, width=d-1;
    if (d==1) { stats->seconds=now_seconds()-started; return count_u64(2); }
    uint64_t full=(UINT64_C(1)<<(unsigned)total)-1U;
    uint64_t bad[50]={0};
    for (int x=0;x<total;++x) for (int y=0;y<total;++y)
        if (x-y>-d && x-y<d) bad[x]|=UINT64_C(1)<<(unsigned)y;

    Budget budget={checked_mul((size_t)memory_mib,1048576U),0,0};
    Map current,next; map_init(&current,&budget); map_init(&next,&budget);
    InitContext init={&current,bad,total,width,symmetry};
    initialize_rec(&init,0,0,0);
    prune(&current,full,width,d,bad,stats); update_stats(stats,&current,&budget);
    size_t previous=0;
    for (int length=width; length<total && current.size; ++length) {
        size_t predicted=current.size;
        if (previous) {
            if (current.size && current.size>SIZE_MAX/current.size)
                die("state prediction overflow");
            size_t raw=current.size*current.size/previous;
            if (raw>current.size) predicted=current.size+(raw-current.size+2U)/3U;
            else predicted=raw;
        }
        prepare_next(&next,capacity_for(predicted)); previous=current.size;
        for (size_t i=0;i<current.capacity;++i) {
            Slot *s=&current.slots[i]; if (!s->used) continue;
            Count value={s->low,s->high};
            uint64_t candidates=full & ~s->used &
                ~forbidden_for_frontier(s->frontier,width,bad);
            while (candidates) {
                uint64_t bit=candidates&(UINT64_C(0)-candidates); candidates^=bit;
                unsigned v=(unsigned)__builtin_ctzll(bit);
                uint64_t used=s->used|bit;
                uint64_t frontier=append_frontier(s->frontier,width,v);
                canonicalize(&used,&frontier,width,total,symmetry);
                map_add(&next,used,frontier,value);
                if (stats->transitions==UINT64_MAX) die("transition counter overflow");
                ++stats->transitions;
            }
        }
        Map tmp=current; current=next; next=tmp;
        prune(&current,full,width,d,bad,stats); update_stats(stats,&current,&budget);
    }
    Count result=count_u64(0);
    for (size_t i=0;i<current.capacity;++i) if (current.slots[i].used) {
        Count x={current.slots[i].low,current.slots[i].high}; count_add(&result,x);
    }
    stats->peak_bytes=budget.peak;
    map_destroy(&current); map_destroy(&next);
    if (budget.in_use) die("DP memory leak");
    stats->seconds=now_seconds()-started;
    return result;
}

static Count compute_value(int d, bool symmetry, uint64_t memory, bool verbose)
{
    Stats s; Count oriented=compute_oriented(d,symmetry,memory,&s);
    Count value=count_half(oriented);
    if (d<=KNOWN_MAX_D && !count_equal(value,parse_count(known[d]))) {
        fprintf(stderr,"error: A318790 mismatch at n=%d: got ",d);
        print_count(stderr,value); fprintf(stderr,", expected %s\n",known[d]);
        exit(1);
    }
    if (verbose) fprintf(stderr,"318790_01: n=%d, N=%d, %s, states=%zu, "
        "transitions=%" PRIu64 ", pruned=%" PRIu64 ", peak memory=%.1f MiB, %.3f s\n",
        d,d*d+1,symmetry?"complement-quotient DP":"plain DP",s.peak_states,
        s.transitions,s.pruned,(double)s.peak_bytes/1048576.0,s.seconds);
    return value;
}

typedef struct { int d,total,width; bool used[10]; int p[10]; uint64_t count; } Direct;

static void direct_rec(Direct *c, int length)
{
    if (length==c->total) { if (c->count==UINT64_MAX) die("direct count overflow"); ++c->count; return; }
    for (int v=0;v<c->total;++v) if (!c->used[v]) {
        bool ok=true;
        for (int j=1;j<=c->width && j<=length;++j) {
            int diff=v-c->p[length-j]; if (diff>-c->d && diff<c->d) {ok=false;break;}
        }
        if (ok) { c->used[v]=true;c->p[length]=v;direct_rec(c,length+1);c->used[v]=false; }
    }
}

static Count direct_value(int d)
{
    Direct c={0}; c.d=d;c.total=d*d+1;c.width=d-1; direct_rec(&c,0);
    if (c.count&1U) die("direct oriented count odd"); return count_u64(c.count/2U);
}

static int check_all(uint64_t memory)
{
    for (int d=1;d<=KNOWN_MAX_D;++d) {
        Count q=compute_value(d,true,memory,false);
        if (d<=4) {
            Count p=compute_value(d,false,memory,false);
            if (!count_equal(q,p)) die("quotient/plain mismatch");
        }
        if (d<=3 && !count_equal(q,direct_value(d))) die("DP/direct mismatch");
    }
    puts("ok: A318790 terms n=1..6; quotient/plain DP n=1..4; independent backtracking n=1..3");
    return 0;
}

static char *beside_executable(const char *argv0,const char *name)
{
    char exe[PATH_MAX],resolved[PATH_MAX]; bool found=false;
#ifdef __APPLE__
    uint32_t size=(uint32_t)sizeof(exe); if (_NSGetExecutablePath(exe,&size)==0) found=true;
#elif defined(__linux__)
    ssize_t n=readlink("/proc/self/exe",exe,sizeof(exe)-1U);
    if (n>=0) {exe[n]='\0';found=true;}
#endif
    if (!found) {size_t n=strlen(argv0);if(n>=sizeof(exe))die("path too long");memcpy(exe,argv0,n+1);}
    const char *base=realpath(exe,resolved);if(!base)base=exe;
    const char *slash=strrchr(base,'/');size_t dir=slash?(size_t)(slash-base):1U;
    if(slash==base)dir=1U;size_t len=strlen(name);
    char *p=malloc(dir+len+2U);if(!p)die("path allocation failed");
    if(!slash)p[0]='.';else if(slash==base)p[0]='/';else memcpy(p,base,dir);
    p[dir]='/';memcpy(p+dir+1U,name,len+1U);return p;
}

static void write_file(const char *argv0,int maxd,bool symmetry,uint64_t memory)
{
    char *final=beside_executable(argv0,"b318790_01.txt");
    char *part=beside_executable(argv0,"b318790_01_part.txt");
    FILE *f=fopen(part,"w");if(!f){fprintf(stderr,"error: cannot open %s: %s\n",part,strerror(errno));exit(1);}
    for(int d=OFFSET;d<=maxd;++d){Count x=compute_value(d,symmetry,memory,true);
        if(fprintf(f,"%d ",d)<0||print_count(f,x)<0||fputc('\n',f)==EOF||fflush(f))die("b-file write failed");}
    if(fclose(f))die("b-file close failed");if(rename(part,final))die("b-file rename failed");
    printf("wrote %s (n=%d..%d)\n",final,OFFSET,maxd);free(final);free(part);
}

static void usage(const char *p)
{
    fprintf(stderr,"usage: %s [MAX_N] [--memory-mib M] [--no-symmetry]\n"
        "       %s --upto MAX_N [options]\n       %s --term N [options]\n"
        "       %s --check [--memory-mib M]\nN is in 1..7; default --upto 6.\n",
        p,p,p,p);
}

int main(int argc,char **argv)
{
    Mode mode=MODE_UPTO;int d=-1;bool have_d=false,have_mode=false,symmetry=true;
    uint64_t memory=DEFAULT_MEMORY_MIB;bool have_memory=false;
    for(int i=1;i<argc;++i){const char *s=argv[i];
        if(!strcmp(s,"--help")||!strcmp(s,"-h")){usage(argv[0]);return 0;}
        if(!strcmp(s,"--memory-mib")){if(have_memory||++i>=argc){usage(argv[0]);return 1;}memory=parse_memory(argv[i]);have_memory=true;}
        else if(!strcmp(s,"--no-symmetry")){symmetry=false;}
        else if(!strcmp(s,"--term")||!strcmp(s,"--upto")){if(have_mode||have_d||++i>=argc){usage(argv[0]);return 1;}mode=!strcmp(s,"--term")?MODE_TERM:MODE_UPTO;have_mode=true;d=parse_d(argv[i],"N");have_d=true;}
        else if(!strcmp(s,"--check")){if(have_mode||have_d){usage(argv[0]);return 1;}mode=MODE_CHECK;have_mode=true;}
        else if(s[0]=='-'||have_d){usage(argv[0]);return 1;}
        else {d=parse_d(s,"MAX_N");have_d=true;}
    }
    if(mode==MODE_CHECK)return check_all(memory);
    if(!have_d)d=DEFAULT_MAX_D;
    if(mode==MODE_TERM){Count x=compute_value(d,symmetry,memory,true);printf("%d ",d);print_count(stdout,x);putchar('\n');return 0;}
    write_file(argv[0],d,symmetry,memory);return 0;
}
