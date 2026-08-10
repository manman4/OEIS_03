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
 *   - N<=65 (d<=8); the used set is unsigned 128-bit and frontier values
 *     use seven bits;
 *   - coefficients use checked unsigned 128-bit arithmetic: an overflow is
 *     reported, never silently wrapped;
 *   - hash growth, allocation sizes, and the user memory budget are checked;
 *   - a partial b-file is renamed only after the whole requested range ends.
 *
 * Verification:
 *   - reference terms a(1)..a(8) are built in; a(7),a(8) were independently
 *     obtained by 318790_04.c;
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
 *   ./318790_01                  # write a(1)..a(7)
 *   ./318790_01 --upto 8 --memory-mib 6000
 *   ./318790_01 --term 8 --memory-mib 6000
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
#define DEFAULT_MAX_D 7
#define MAX_D 8
#define KNOWN_MAX_D 8
#define DEFAULT_CHECK_D 4
#define DEFAULT_MEMORY_MIB UINT64_C(1024)
#define MIN_MEMORY_MIB UINT64_C(16)
#define INITIAL_CAPACITY ((size_t)16)
#define LOAD_NUM ((size_t)7)
#define LOAD_DEN ((size_t)8)
#define VALUE_BITS 7U
#define VALUE_MASK UINT64_C(127)
#define LOOKAHEAD_MULTIPLIER 2

static const char *const known[KNOWN_MAX_D + 1] = {
    NULL, "1", "7", "20", "37", "64", "109", "188", "333"
};

typedef struct { uint64_t low, high; } Count;
typedef uint128_t Bits;
typedef struct {
    uint64_t used_low;   /* both used words zero denote an empty slot */
    uint64_t used_high;
    uint64_t frontier;   /* oldest value in the least significant field */
    uint64_t low, high;
} Slot;
_Static_assert(sizeof(Slot)==40U,"state slot must remain five words");
typedef struct { size_t limit, in_use, peak; } Budget;
typedef struct {
    Slot *slots;
    size_t capacity, size;
    Budget *budget;
} Map;
typedef struct {
    uint64_t transitions, pruned,capacity_pruned,direct_nodes;
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

static Count count_multiply_u64(Count a,uint64_t multiplier)
{
    uint128_t value=((uint128_t)a.high<<64U)|a.low;
    if(multiplier!=0U&&value>(~(uint128_t)0)/(uint128_t)multiplier)
        die("128-bit coefficient multiplication overflow");
    value*=multiplier;
    Count result={(uint64_t)value,(uint64_t)(value>>64U)};
    return result;
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

static bool slot_empty(const Slot *s)
{ return s->used_low==0U&&s->used_high==0U; }

static Bits slot_used(const Slot *s)
{ return ((Bits)s->used_high<<64U)|s->used_low; }

static size_t key_index(Bits used, uint64_t frontier, size_t mask)
{
    return (size_t)mix64((uint64_t)used^mix64((uint64_t)(used>>64U))^
                         mix64(frontier))&mask;
}

static Slot *map_find(Map *m, Bits used, uint64_t frontier)
{
    size_t i = key_index(used, frontier, m->capacity - 1U);
    for (;;) {
        Slot *s = &m->slots[i];
        if (slot_empty(s)||(slot_used(s)==used&&s->frontier==frontier))
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
    for (size_t i=0; i<oldcap; ++i) if (!slot_empty(&old[i])) {
        Slot *s = map_find(m, slot_used(&old[i]), old[i].frontier);
        *s = old[i];
    }
    release_slots(m->budget, old, oldcap);
}

static void map_add(Map *m, Bits used, uint64_t frontier, Count add)
{
    Slot *s = map_find(m, used, frontier);
    if (!slot_empty(s)) {
        Count c = {s->low, s->high}; count_add(&c, add);
        s->low = c.low; s->high = c.high; return;
    }
    if (m->size + 1U > (m->capacity/LOAD_DEN)*LOAD_NUM) {
        map_grow(m); s = map_find(m, used, frontier);
    }
    s->used_low=(uint64_t)used;s->used_high=(uint64_t)(used>>64U);
    s->frontier=frontier;s->low=add.low;s->high=add.high;
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

static Bits reverse_low(Bits x, int bits)
{
    Bits reversed=((Bits)reverse_bits((uint64_t)x)<<64U)|
                  reverse_bits((uint64_t)(x>>64U));
    return reversed>>(128U-(unsigned)bits);
}

static uint64_t complement_frontier(uint64_t f, int width, int total)
{
    uint64_t r=0;
    for (int i=0;i<width;++i) {
        unsigned v=(unsigned)((f>>(VALUE_BITS*(unsigned)i))&VALUE_MASK);
        r |= (uint64_t)(total-1-(int)v) << (VALUE_BITS*(unsigned)i);
    }
    return r;
}

static void canonicalize(Bits *used, uint64_t *frontier,
                         int width, int total, bool symmetry)
{
    if (!symmetry) return;
    Bits cu=reverse_low(*used,total);
    uint64_t cf=complement_frontier(*frontier,width,total);
    if (cu < *used || (cu == *used && cf < *frontier)) {
        *used=cu; *frontier=cf;
    }
}

static Bits forbidden_for_frontier(uint64_t f, int width,
                                   const Bits *bad)
{
    Bits r=0;
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

static unsigned bit_index(Bits bit)
{
    uint64_t low=(uint64_t)bit;
    return low?(unsigned)__builtin_ctzll(low)
              :64U+(unsigned)__builtin_ctzll((uint64_t)(bit>>64U));
}

static unsigned popcount_bits(Bits value)
{
    return (unsigned)__builtin_popcountll((uint64_t)value)+
           (unsigned)__builtin_popcountll((uint64_t)(value>>64U));
}

static bool capacity_possible(Bits used,uint64_t frontier,int frontier_count,
                              int total,int index,int length,
                              const Bits *value_windows)
{
    int remaining_positions=total-length;
    for(int start=0;start<=total-index;++start){
        Bits clique=value_windows[start];
        int remaining=(int)popcount_bits(clique&~used);
        if(remaining==0)continue;
        int first=1;
        for(int position=frontier_count-1;position>=0;--position){
            unsigned value=(unsigned)((frontier>>
                (VALUE_BITS*(unsigned)position))&VALUE_MASK);
            if(clique&((Bits)1<<value)){
                int age=frontier_count-1-position;
                first=index-age;
                break;
            }
        }
        if(first+index*(remaining-1)>remaining_positions)return false;
    }
    return true;
}

static bool has_extension(Bits remaining, uint64_t frontier, int width,
                          const Bits *bad,int depth,int total,int index,
                          const Bits *value_windows)
{
    if (!depth) return true;
    Bits candidates=remaining&~forbidden_for_frontier(frontier,width,bad);
    int next_length=total-(int)popcount_bits(remaining)+1;
    while (candidates) {
        Bits bit=candidates&((Bits)0-candidates);candidates^=bit;
        unsigned v=bit_index(bit);
        Bits next_remaining=remaining^bit;
        Bits next_used=(((Bits)1<<(unsigned)total)-1U)^next_remaining;
        uint64_t next_frontier=append_frontier(frontier,width,v);
        if(!capacity_possible(next_used,next_frontier,width,total,index,
                              next_length,value_windows))continue;
        if(has_extension(next_remaining,next_frontier,width,bad,depth-1,
                         total,index,value_windows))return true;
    }
    return false;
}

static uint64_t count_direct_completions(Bits used,uint64_t frontier,
                                         Bits full,int width,int total,
                                         int index,const Bits *bad,
                                         const Bits *value_windows,
                                         Stats *stats)
{
    if(stats->direct_nodes==UINT64_MAX)die("direct-node counter overflow");
    ++stats->direct_nodes;
    int length=(int)popcount_bits(used);
    if(length==total)return UINT64_C(1);
    Bits candidates=full&~used&~forbidden_for_frontier(frontier,width,bad);
    uint64_t result=0;
    while(candidates){
        Bits bit=candidates&((Bits)0-candidates);candidates^=bit;
        unsigned value=bit_index(bit);
        Bits next_used=used|bit;
        uint64_t next_frontier=append_frontier(frontier,width,value);
        if(!capacity_possible(next_used,next_frontier,width,total,index,
                              length+1,value_windows))continue;
        uint64_t add=count_direct_completions(next_used,next_frontier,full,
            width,total,index,bad,value_windows,stats);
        if(UINT64_MAX-result<add)die("direct-completion count overflow");
        result+=add;
    }
    return result;
}

static void update_stats(Stats *s, const Map *m, const Budget *b)
{
    if (s->peak_states<m->size) s->peak_states=m->size;
    if (s->peak_bytes<b->peak) s->peak_bytes=b->peak;
}

typedef struct {
    Map *map;const Bits *bad;const Bits *value_windows;
    int total,width,index;
    Stats *stats;
    bool symmetry;
} InitContext;

static void initialize_rec(InitContext *c, int length, Bits used,
                           uint64_t frontier)
{
    if (length==c->width) {
        Bits full=((Bits)1<<(unsigned)c->total)-1U;
        Bits remaining=full&~used;
        int left=(int)popcount_bits(remaining);
        int horizon=LOOKAHEAD_MULTIPLIER*c->index;
        int depth=left<horizon?left:horizon;
        if(!has_extension(remaining,frontier,c->width,c->bad,depth,c->total,
                          c->index,c->value_windows)){
            if(c->stats->pruned==UINT64_MAX)die("lookahead-prune counter overflow");
            ++c->stats->pruned;
            return;
        }
        canonicalize(&used,&frontier,c->width,c->total,c->symmetry);
        map_add(c->map,used,frontier,count_u64(1)); return;
    }
    Bits full=((Bits)1<<(unsigned)c->total)-1U;
    Bits candidates=full&~used;
    if (length) candidates &= ~forbidden_for_frontier(frontier,length,c->bad);
    while (candidates) {
        Bits bit=candidates&((Bits)0-candidates);candidates^=bit;
        unsigned v=bit_index(bit);
        uint64_t nf=frontier | ((uint64_t)v << (VALUE_BITS*(unsigned)length));
        Bits next_used=used|bit;
        if(!capacity_possible(next_used,nf,length+1,c->total,c->index,
                              length+1,c->value_windows)){
            if(c->stats->capacity_pruned==UINT64_MAX)
                die("capacity-prune counter overflow");
            ++c->stats->capacity_pruned;
            continue;
        }
        initialize_rec(c,length+1,next_used,nf);
    }
}

static Count compute_oriented(int d, bool symmetry, uint64_t memory_mib,
                              Stats *stats)
{
    double started=now_seconds(); memset(stats,0,sizeof(*stats));
    int total=d*d+1, width=d-1;
    if (d==1) { stats->seconds=now_seconds()-started; return count_u64(2); }
    Bits full=((Bits)1<<(unsigned)total)-1U;
    Bits bad[65]={0},value_windows[65]={0};
    for (int x=0;x<total;++x) for (int y=0;y<total;++y)
        if (x-y>-d && x-y<d) bad[x]|=(Bits)1<<(unsigned)y;
    for(int start=0;start<=total-d;++start)
        for(int value=start;value<start+d;++value)
            value_windows[start]|=(Bits)1<<(unsigned)value;

    Budget budget={checked_mul((size_t)memory_mib,1048576U),0,0};
    Map current,next; map_init(&current,&budget); map_init(&next,&budget);
    InitContext init={&current,bad,value_windows,total,width,d,stats,symmetry};
    initialize_rec(&init,0,0,0);
    update_stats(stats,&current,&budget);
    size_t previous=0;
    Count result=count_u64(0);
    bool finished_directly=false;
    for (int length=width; length<total && current.size; ++length) {
        if(total-length<=LOOKAHEAD_MULTIPLIER*d){
            for(size_t i=0;i<current.capacity;++i){
                Slot *s=&current.slots[i];if(slot_empty(s))continue;
                uint64_t completions=count_direct_completions(slot_used(s),
                    s->frontier,full,width,total,d,bad,value_windows,stats);
                Count coefficient={s->low,s->high};
                count_add(&result,count_multiply_u64(coefficient,completions));
            }
            finished_directly=true;
            break;
        }
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
            Slot *s=&current.slots[i];if(slot_empty(s))continue;
            Count value={s->low,s->high};
            Bits state_used=slot_used(s);
            Bits candidates=full&~state_used&
                ~forbidden_for_frontier(s->frontier,width,bad);
            while (candidates) {
                Bits bit=candidates&((Bits)0-candidates);candidates^=bit;
                unsigned v=bit_index(bit);
                Bits used=state_used|bit;
                uint64_t frontier=append_frontier(s->frontier,width,v);
                if(!capacity_possible(used,frontier,width,total,d,length+1,
                                      value_windows)){
                    if(stats->capacity_pruned==UINT64_MAX)
                        die("capacity-prune counter overflow");
                    ++stats->capacity_pruned;
                    continue;
                }
                Bits remaining=full&~used;
                int left=(int)popcount_bits(remaining);
                int horizon=LOOKAHEAD_MULTIPLIER*d;
                int depth=left<horizon?left:horizon;
                if(!has_extension(remaining,frontier,width,bad,depth,total,d,
                                  value_windows)){
                    if(stats->pruned==UINT64_MAX)
                        die("lookahead-prune counter overflow");
                    ++stats->pruned;
                    continue;
                }
                canonicalize(&used,&frontier,width,total,symmetry);
                map_add(&next,used,frontier,value);
                if (stats->transitions==UINT64_MAX) die("transition counter overflow");
                ++stats->transitions;
            }
        }
        Map tmp=current; current=next; next=tmp;
        update_stats(stats,&current,&budget);
    }
    if(!finished_directly)
        for(size_t i=0;i<current.capacity;++i)if(!slot_empty(&current.slots[i])){
            Count x={current.slots[i].low,current.slots[i].high};count_add(&result,x);
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
        "transitions=%" PRIu64 ", lookahead pruned=%" PRIu64
        ", capacity pruned=%" PRIu64 ", direct nodes=%" PRIu64
        ", peak memory=%.1f MiB, %.3f s\n",
        d,d*d+1,symmetry?"complement-quotient DP":"plain DP",s.peak_states,
        s.transitions,s.pruned,s.capacity_pruned,s.direct_nodes,
        (double)s.peak_bytes/1048576.0,s.seconds);
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
    Bits low=(Bits)1,high=(Bits)1<<64U;
    if(reverse_low(low,65)!=high||reverse_low(high,65)!=low||
       popcount_bits(low|high)!=2U)
        die("128-bit state-set self-test failed");
    for (int d=1;d<=DEFAULT_CHECK_D;++d) {
        Count q=compute_value(d,true,memory,false);
        if (d<=4) {
            Count p=compute_value(d,false,memory,false);
            if (!count_equal(q,p)) die("quotient/plain mismatch");
        }
        if (d<=3 && !count_equal(q,direct_value(d))) die("DP/direct mismatch");
    }
    puts("ok: A318790 terms and quotient/plain DP n=1..4; independent backtracking n=1..3");
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
        "       %s --check [--memory-mib M]\nN is in 1..8; default --upto 7.\n",
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
