/*
 * A318790 -- exact meet-in-the-middle subset DP.
 *
 * For index d put N=d^2+1 and w=d-1.  This program counts permutations p
 * of [N] satisfying |p(i+j)-p(i)|>=d for 1<=j<=w, and divides by two for
 * reversal.  Unlike 318790_01.c, it stops prefix propagation at the middle:
 * independently counted left and reversed-right halves are joined there.
 *
 * A half-path state is (used set, ordered last-w values).  If the left
 * boundary is a[0],...,a[w-1] (oldest to newest) and the original right
 * boundary is b[0],...,b[w-1], the cross-cut tests are
 *
 *     |a[i]-b[j]| >= d,       0 <= j <= i < w.
 *
 * Right states were built after reversing the right half, so their frontier
 * fields are reversed before these tests.  Only complementary used masks are
 * joined.  For even N, complementary split masks are reversal-paired; only
 * one order is joined and the result is doubled.
 *
 * Value complementation x->N-1-x quotients the half tables for d>=3.  Such a
 * state cannot be fixed because its w>=2 distinct ordered frontier entries
 * would each have to be the unique middle value.  Records are expanded back
 * to the two exact orbit members before joining.  d=1,2 use independent
 * direct backtracking.
 *
 * Exact pruning: a state is discarded only if an explicit search proves
 * that it has no continuation through the next min(2d,remaining) positions.
 * Every full permutation has such a continuation, so no answer is lost.
 *
 * Limits and safety:
 *   - 1<=d<=7, hence N<=50 and all sets fit uint64_t;
 *   - all coefficients and products use checked unsigned 128-bit arithmetic;
 *   - overflow, allocation overflow, or the --memory-mib limit stops with an
 *     error instead of returning a wrapped value;
 *   - the partial b-file is atomically renamed only after successful finish.
 *
 * Reference: https://oeis.org/A318790
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *     318790_02.c -o 318790_02
 *
 * Usage:
 *   ./318790_02 --term 6 --memory-mib 6000
 *   ./318790_02 --upto 6 --memory-mib 6000
 *   ./318790_02 --check 4
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
#error "318790_02 requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 u128;

#define OFFSET 1
#define MAX_D 7
#define DEFAULT_MAX_D 6
#define KNOWN_MAX_D 6
#define DEFAULT_CHECK_D 4
#define DEFAULT_MEMORY_MIB UINT64_C(1024)
#define MIN_MEMORY_MIB UINT64_C(16)
#define INITIAL_CAPACITY ((size_t)16)
#define LOAD_NUM ((size_t)7)
#define LOAD_DEN ((size_t)8)
#define VALUE_BITS 6U
#define VALUE_MASK UINT64_C(63)

static const char *const known[KNOWN_MAX_D+1]={NULL,"1","7","20","37","64","109"};
typedef struct {uint64_t lo,hi;} Count;
typedef struct {uint64_t used,frontier,lo,hi;} Slot;
typedef struct {size_t limit,in_use,peak;} Budget;
typedef struct {Slot *slots;size_t cap,size;Budget *budget;} Map;
typedef struct {
    uint64_t transitions,pruned,split_masks,joined_boundaries;
    size_t left_states,right_states,peak_bytes;
    double build_seconds,join_seconds,total_seconds;
} Stats;
typedef struct {uint64_t used,frontier,lo,hi;} Record;
typedef struct {Record *v;size_t count,bytes;Budget *budget;} Records;
typedef enum {MODE_UPTO,MODE_TERM,MODE_CHECK} Mode;

static void die(const char *s){fprintf(stderr,"error: %s\n",s);exit(1);}
static double seconds_now(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))die("clock_gettime failed");return(double)t.tv_sec+(double)t.tv_nsec/1e9;}
static size_t mul_size(size_t a,size_t b){if(a&&b>SIZE_MAX/a)die("allocation size overflow");return a*b;}
static Count zero_count(void){Count x={0,0};return x;}
static Count u64_count(uint64_t x){Count c={x,0};return c;}
static bool equal_count(Count a,Count b){return a.lo==b.lo&&a.hi==b.hi;}
static void add_count(Count *a,Count b){uint64_t old=a->lo;a->lo+=b.lo;uint64_t carry=a->lo<old;uint64_t h=a->hi+b.hi;if(h<a->hi||h+carry<h)die("128-bit addition overflow");a->hi=h+carry;}
static Count half_count(Count a){if(a.lo&1U)die("odd count where an even count is required");Count b={(a.lo>>1)|(a.hi<<63),a.hi>>1};return b;}
static Count multiply_count(Count a,Count b){
    if(a.hi&&b.hi)die("128-bit multiplication overflow");
    u128 base=(u128)a.lo*b.lo;
    u128 cross=(u128)a.hi*b.lo+(u128)a.lo*b.hi;
    if(cross>UINT64_MAX)die("128-bit multiplication overflow");
    uint64_t high=(uint64_t)(base>>64U);
    if((uint64_t)cross>UINT64_MAX-high)die("128-bit multiplication overflow");
    Count c={(uint64_t)base,high+(uint64_t)cross};return c;
}
static Count parse_count(const char *s){u128 x=0,max=~(u128)0;for(;*s;++s){if(*s<'0'||*s>'9')die("bad built-in term");unsigned z=(unsigned)(*s-'0');if(x>(max-z)/10U)die("built-in term overflow");x=x*10U+z;}Count c={(uint64_t)x,(uint64_t)(x>>64U)};return c;}
static int print_count(FILE *f,Count a){char b[40];size_t n=0;u128 x=((u128)a.hi<<64U)|a.lo;do{b[n++]=(char)('0'+(unsigned)(x%10U));x/=10U;}while(x);while(n)if(fputc(b[--n],f)==EOF)return-1;return 0;}
static int parse_d(const char *s,const char *label){char *e=NULL;errno=0;long x=strtol(s,&e,10);if(errno||e==s||*e||x<OFFSET||x>MAX_D){fprintf(stderr,"error: %s must be in 1..7: %s\n",label,s);exit(1);}return(int)x;}
static uint64_t parse_memory(const char *s){char *e=NULL;errno=0;unsigned long long x=strtoull(s,&e,10);uint64_t max=(uint64_t)(SIZE_MAX/1048576U);if(errno||e==s||*e||x<MIN_MEMORY_MIB||x>max){fprintf(stderr,"error: MEMORY_MIB must be in %"PRIu64"..%"PRIu64": %s\n",MIN_MEMORY_MIB,max,s);exit(1);}return(uint64_t)x;}

static void budget_take(Budget *b,size_t bytes){if(bytes>b->limit-b->in_use){fprintf(stderr,"error: memory budget exceeded: need %.1f MiB with %.1f/%.1f MiB in use\n",(double)bytes/1048576.0,(double)b->in_use/1048576.0,(double)b->limit/1048576.0);exit(1);}b->in_use+=bytes;if(b->peak<b->in_use)b->peak=b->in_use;}
static Slot *alloc_slots(Budget *b,size_t cap){size_t bytes=mul_size(cap,sizeof(Slot));budget_take(b,bytes);Slot *p=calloc(cap,sizeof(*p));if(!p)die("DP allocation failed");return p;}
static void free_slots(Budget *b,Slot *p,size_t cap){free(p);size_t bytes=mul_size(cap,sizeof(Slot));if(bytes>b->in_use)die("memory budget underflow");b->in_use-=bytes;}
static uint64_t mix64(uint64_t x){x^=x>>30;x*=UINT64_C(0xbf58476d1ce4e5b9);x^=x>>27;x*=UINT64_C(0x94d049bb133111eb);return x^(x>>31);}
static size_t hash_index(uint64_t u,uint64_t f,size_t mask){return(size_t)mix64(u^mix64(f))&mask;}
static Slot *map_find(Map *m,uint64_t u,uint64_t f){size_t i=hash_index(u,f,m->cap-1);for(;;){Slot *s=&m->slots[i];if(!s->used||(s->used==u&&s->frontier==f))return s;i=(i+1)&(m->cap-1);}}
static void map_init_cap(Map *m,Budget *b,size_t cap){if(cap<INITIAL_CAPACITY||(cap&(cap-1)))die("invalid hash capacity");m->budget=b;m->cap=cap;m->size=0;m->slots=alloc_slots(b,cap);}
static void map_init(Map *m,Budget *b){map_init_cap(m,b,INITIAL_CAPACITY);}
static void map_destroy(Map *m){if(m->slots)free_slots(m->budget,m->slots,m->cap);m->slots=NULL;m->cap=m->size=0;}
static void map_grow(Map *m){if(m->cap>SIZE_MAX/2)die("hash capacity overflow");size_t oldcap=m->cap;Slot *old=m->slots;m->cap*=2;m->slots=alloc_slots(m->budget,m->cap);for(size_t i=0;i<oldcap;++i)if(old[i].used)*map_find(m,old[i].used,old[i].frontier)=old[i];free_slots(m->budget,old,oldcap);}
static void map_add(Map *m,uint64_t u,uint64_t f,Count x){Slot *s=map_find(m,u,f);if(s->used){Count a={s->lo,s->hi};add_count(&a,x);s->lo=a.lo;s->hi=a.hi;return;}if(m->size+1>(m->cap/LOAD_DEN)*LOAD_NUM){map_grow(m);s=map_find(m,u,f);}*s=(Slot){u,f,x.lo,x.hi};++m->size;}
static void map_clear(Map *m){memset(m->slots,0,mul_size(m->cap,sizeof(Slot)));m->size=0;}
static size_t capacity_for(size_t entries){size_t c=INITIAL_CAPACITY;while((c/LOAD_DEN)*LOAD_NUM<entries){if(c>SIZE_MAX/2)die("capacity prediction overflow");c*=2;}return c;}
static void prepare_next(Map *m,size_t cap){if(cap<INITIAL_CAPACITY)cap=INITIAL_CAPACITY;if(m->cap==cap){map_clear(m);return;}Budget *b=m->budget;map_destroy(m);map_init_cap(m,b,cap);}

static uint64_t reverse_bits(uint64_t x){x=((x&UINT64_C(0x5555555555555555))<<1)|((x>>1)&UINT64_C(0x5555555555555555));x=((x&UINT64_C(0x3333333333333333))<<2)|((x>>2)&UINT64_C(0x3333333333333333));x=((x&UINT64_C(0x0f0f0f0f0f0f0f0f))<<4)|((x>>4)&UINT64_C(0x0f0f0f0f0f0f0f0f));x=((x&UINT64_C(0x00ff00ff00ff00ff))<<8)|((x>>8)&UINT64_C(0x00ff00ff00ff00ff));x=((x&UINT64_C(0x0000ffff0000ffff))<<16)|((x>>16)&UINT64_C(0x0000ffff0000ffff));return(x<<32)|(x>>32);}
static uint64_t reverse_low(uint64_t x,int bits){return reverse_bits(x)>>(64U-(unsigned)bits);}
static uint64_t complement_frontier(uint64_t f,int w,int total){uint64_t r=0;for(int i=0;i<w;++i){unsigned v=(unsigned)((f>>(VALUE_BITS*(unsigned)i))&VALUE_MASK);r|=(uint64_t)(total-1-(int)v)<<(VALUE_BITS*(unsigned)i);}return r;}
static void canonicalize(uint64_t *u,uint64_t *f,int w,int total,bool symmetry){if(!symmetry)return;uint64_t cu=reverse_low(*u,total),cf=complement_frontier(*f,w,total);if(cu<*u||(cu==*u&&cf<*f)){*u=cu;*f=cf;}}
static uint64_t forbidden(uint64_t f,int w,const uint64_t *bad){uint64_t r=0;for(int i=0;i<w;++i){unsigned v=(unsigned)((f>>(VALUE_BITS*(unsigned)i))&VALUE_MASK);r|=bad[v];}return r;}
static uint64_t append_frontier(uint64_t f,int w,unsigned v){return w==1?v:(f>>VALUE_BITS)|((uint64_t)v<<(VALUE_BITS*(unsigned)(w-1)));}
static bool has_extension(uint64_t rem,uint64_t f,int w,const uint64_t *bad,int depth){if(!depth)return true;uint64_t c=rem&~forbidden(f,w,bad);while(c){uint64_t bit=c&(UINT64_C(0)-c);c^=bit;unsigned v=(unsigned)__builtin_ctzll(bit);if(has_extension(rem^bit,append_frontier(f,w,v),w,bad,depth-1))return true;}return false;}
static void prune(Map *m,uint64_t full,int w,int d,const uint64_t *bad,Stats *st){for(size_t i=0;i<m->cap;++i){Slot *s=&m->slots[i];if(!s->used)continue;uint64_t rem=full&~s->used;int left=__builtin_popcountll(rem),h=2*d,depth=left<h?left:h;if(!has_extension(rem,s->frontier,w,bad,depth)){/* This map is only scanned before it is cleared, so deleting an open-addressing slot cannot affect a later key lookup. */s->used=s->frontier=s->lo=s->hi=0;--m->size;if(st->pruned==UINT64_MAX)die("prune counter overflow");++st->pruned;}}}

typedef struct {Map *map;const uint64_t *bad;int total,w;bool symmetry;} Init;
static void init_rec(Init *c,int length,uint64_t used,uint64_t f){if(length==c->w){canonicalize(&used,&f,c->w,c->total,c->symmetry);map_add(c->map,used,f,u64_count(1));return;}uint64_t full=(UINT64_C(1)<<(unsigned)c->total)-1,candidates=full&~used;if(length)candidates&=~forbidden(f,length,c->bad);while(candidates){uint64_t bit=candidates&(UINT64_C(0)-candidates);candidates^=bit;unsigned v=(unsigned)__builtin_ctzll(bit);init_rec(c,length+1,used|bit,f|((uint64_t)v<<(VALUE_BITS*(unsigned)length)));}}
static void extend(const Map *current,Map *next,uint64_t full,int w,int total,const uint64_t *bad,bool symmetry,Stats *st){for(size_t i=0;i<current->cap;++i){const Slot *s=&current->slots[i];if(!s->used)continue;Count x={s->lo,s->hi};uint64_t c=full&~s->used&~forbidden(s->frontier,w,bad);while(c){uint64_t bit=c&(UINT64_C(0)-c);c^=bit;unsigned v=(unsigned)__builtin_ctzll(bit);uint64_t u=s->used|bit,f=append_frontier(s->frontier,w,v);canonicalize(&u,&f,w,total,symmetry);map_add(next,u,f,x);if(st->transitions==UINT64_MAX)die("transition counter overflow");++st->transitions;}}}

static int compare_record(const void *aa,const void *bb){const Record *a=aa,*b=bb;if(a->used!=b->used)return a->used<b->used?-1:1;if(a->frontier!=b->frontier)return a->frontier<b->frontier?-1:1;return 0;}
static Records collect(Map *m,int w,int total,bool symmetry){Records r={0};r.budget=m->budget;size_t multiplier=symmetry?2U:1U;r.count=mul_size(m->size,multiplier);size_t alloc=r.count?r.count:1;r.bytes=mul_size(alloc,sizeof(Record));budget_take(r.budget,r.bytes);r.v=malloc(r.bytes);if(!r.v)die("record allocation failed");size_t j=0;for(size_t i=0;i<m->cap;++i){Slot *s=&m->slots[i];if(!s->used)continue;Count x={s->lo,s->hi};if(symmetry)x=half_count(x);r.v[j++]=(Record){s->used,s->frontier,x.lo,x.hi};if(symmetry)r.v[j++]=(Record){reverse_low(s->used,total),complement_frontier(s->frontier,w,total),x.lo,x.hi};}if(j!=r.count)die("record count mismatch");qsort(r.v,r.count,sizeof(Record),compare_record);return r;}
static void free_records(Records *r){free(r->v);if(r->bytes>r->budget->in_use)die("record budget underflow");r->budget->in_use-=r->bytes;memset(r,0,sizeof(*r));}
static size_t lower_used(const Record *v,size_t count,uint64_t used){size_t first=0,len=count;while(len){size_t half=len/2,mid=first+half;if(v[mid].used<used){first=mid+1;len-=half+1;}else len=half;}return first;}
static bool cross_ok(uint64_t lf,uint64_t rf,int w,const uint64_t *bad){for(int i=0;i<w;++i){unsigned a=(unsigned)((lf>>(VALUE_BITS*(unsigned)i))&VALUE_MASK);for(int j=0;j<=i;++j){unsigned b=(unsigned)((rf>>(VALUE_BITS*(unsigned)(w-1-j)))&VALUE_MASK);if(bad[a]&(UINT64_C(1)<<b))return false;}}return true;}

static Count join(Map *left_map,Map *right_map,uint64_t full,int w,int total,int left_length,const uint64_t *bad,bool symmetry,Stats *st){bool same=left_map==right_map,equal_halves=2*left_length==total;Budget *budget=left_map->budget;Records left=collect(left_map,w,total,symmetry);map_destroy(left_map);Records separate={0},*right;if(same)right=&left;else{separate=collect(right_map,w,total,symmetry);map_destroy(right_map);right=&separate;}Count answer=zero_count();for(size_t lb=0;lb<left.count;){uint64_t subset=left.v[lb].used;size_t le=lb+1;while(le<left.count&&left.v[le].used==subset)++le;uint64_t comp=full^subset;if(!equal_halves||subset<comp){size_t rb=lower_used(right->v,right->count,comp);if(rb<right->count&&right->v[rb].used==comp){size_t re=rb+1;while(re<right->count&&right->v[re].used==comp)++re;if(st->split_masks==UINT64_MAX)die("split counter overflow");++st->split_masks;for(size_t i=lb;i<le;++i)for(size_t j=rb;j<re;++j)if(cross_ok(left.v[i].frontier,right->v[j].frontier,w,bad)){Count a={left.v[i].lo,left.v[i].hi},b={right->v[j].lo,right->v[j].hi};add_count(&answer,multiply_count(a,b));if(st->joined_boundaries==UINT64_MAX)die("join counter overflow");++st->joined_boundaries;}}}lb=le;}if(equal_halves)add_count(&answer,answer);if(st->peak_bytes<budget->peak)st->peak_bytes=budget->peak;if(!same)free_records(&separate);free_records(&left);return answer;}

typedef struct {int d,total,w;bool used[10];int p[10];uint64_t count;} Direct;
static void direct_rec(Direct *c,int len){if(len==c->total){if(c->count==UINT64_MAX)die("direct counter overflow");++c->count;return;}for(int v=0;v<c->total;++v)if(!c->used[v]){bool ok=true;for(int j=1;j<=c->w&&j<=len;++j){int z=v-c->p[len-j];if(z>-c->d&&z<c->d){ok=false;break;}}if(ok){c->used[v]=true;c->p[len]=v;direct_rec(c,len+1);c->used[v]=false;}}}
static Count direct_value(int d){Direct c={0};c.d=d;c.total=d*d+1;c.w=d-1;direct_rec(&c,0);if(c.count&1U)die("direct oriented count odd");return u64_count(c.count/2);}

static Count compute_mitm(int d,bool symmetry,uint64_t memory,Stats *st){double started=seconds_now();memset(st,0,sizeof(*st));if(d<=2){Count x=direct_value(d);st->total_seconds=seconds_now()-started;return x;}int total=d*d+1,w=d-1,left_length=total/2,right_length=total-left_length;uint64_t full=(UINT64_C(1)<<(unsigned)total)-1,bad[50]={0};for(int x=0;x<total;++x)for(int y=0;y<total;++y)if(x-y>-d&&x-y<d)bad[x]|=UINT64_C(1)<<(unsigned)y;Budget budget={mul_size((size_t)memory,1048576U),0,0};Map current,next;map_init(&current,&budget);map_init(&next,&budget);Init init={&current,bad,total,w,symmetry};init_rec(&init,0,0,0);prune(&current,full,w,d,bad,st);size_t previous=0;double build=seconds_now();for(int len=w;len<right_length&&current.size;++len){size_t predicted=current.size;if(previous){if(current.size&&current.size>SIZE_MAX/current.size)die("state prediction overflow");size_t raw=current.size*current.size/previous;if(raw>current.size)predicted=current.size+(raw-current.size+2)/3;else predicted=raw;}prepare_next(&next,capacity_for(predicted));previous=current.size;extend(&current,&next,full,w,total,bad,symmetry,st);Map tmp=current;current=next;next=tmp;prune(&current,full,w,d,bad,st);}st->build_seconds=seconds_now()-build;st->left_states=(left_length==right_length?current.size:next.size);st->right_states=current.size;if(budget.peak>st->peak_bytes)st->peak_bytes=budget.peak;Map *lm,*rm;if(left_length==right_length){lm=rm=&current;map_destroy(&next);}else{lm=&next;rm=&current;}double joining=seconds_now();Count oriented=join(lm,rm,full,w,total,left_length,bad,symmetry,st);st->join_seconds=seconds_now()-joining;map_destroy(&current);map_destroy(&next);if(budget.in_use)die("DP memory leak");st->total_seconds=seconds_now()-started;return half_count(oriented);}
static Count compute_value(int d,bool symmetry,uint64_t memory,bool verbose){Stats st;Count x=compute_mitm(d,symmetry,memory,&st);if(d<=KNOWN_MAX_D&&!equal_count(x,parse_count(known[d]))){fprintf(stderr,"error: A318790 mismatch at n=%d: got ",d);print_count(stderr,x);fprintf(stderr,", expected %s\n",known[d]);exit(1);}if(verbose)fprintf(stderr,"318790_02: n=%d, N=%d, MITM %s, left/right states=%zu/%zu, transitions=%"PRIu64", pruned=%"PRIu64", split masks=%"PRIu64", joined boundaries=%"PRIu64", peak memory=%.1f MiB, build/join/total=%.3f/%.3f/%.3f s\n",d,d*d+1,symmetry?"complement-quotient":"plain",st.left_states,st.right_states,st.transitions,st.pruned,st.split_masks,st.joined_boundaries,(double)st.peak_bytes/1048576.0,st.build_seconds,st.join_seconds,st.total_seconds);return x;}

static int check_all(int maximum,uint64_t memory){for(int d=1;d<=maximum;++d){Count q=compute_value(d,true,memory,false);if(d<=4){Count p=compute_value(d,false,memory,false);if(!equal_count(q,p))die("quotient/plain MITM mismatch");}if(d<=3&&!equal_count(q,direct_value(d)))die("MITM/direct mismatch");}printf("ok: A318790 terms n=1..%d; quotient/plain MITM through n=%d; independent backtracking through n=%d\n",maximum,maximum<4?maximum:4,maximum<3?maximum:3);return 0;}
static char *beside(const char *argv0,const char *name){char exe[PATH_MAX],resolved[PATH_MAX];bool found=false;
#ifdef __APPLE__
uint32_t z=(uint32_t)sizeof(exe);if(_NSGetExecutablePath(exe,&z)==0)found=true;
#elif defined(__linux__)
ssize_t z=readlink("/proc/self/exe",exe,sizeof(exe)-1);if(z>=0){exe[z]='\0';found=true;}
#endif
if(!found){size_t z=strlen(argv0);if(z>=sizeof(exe))die("path too long");memcpy(exe,argv0,z+1);}const char *base=realpath(exe,resolved);if(!base)base=exe;const char *slash=strrchr(base,'/');size_t dir=slash?(size_t)(slash-base):1;if(slash==base)dir=1;size_t len=strlen(name);char *p=malloc(dir+len+2);if(!p)die("path allocation failed");if(!slash)p[0]='.';else if(slash==base)p[0]='/';else memcpy(p,base,dir);p[dir]='/';memcpy(p+dir+1,name,len+1);return p;}
static void write_file(const char *argv0,int maximum,bool symmetry,uint64_t memory){char *final=beside(argv0,"b318790_02.txt"),*part=beside(argv0,"b318790_02_part.txt");FILE *f=fopen(part,"w");if(!f)die("cannot open partial b-file");for(int d=OFFSET;d<=maximum;++d){Count x=compute_value(d,symmetry,memory,true);if(fprintf(f,"%d ",d)<0||print_count(f,x)<0||fputc('\n',f)==EOF||fflush(f))die("b-file write failed");}if(fclose(f))die("b-file close failed");if(rename(part,final))die("b-file rename failed");printf("wrote %s (n=1..%d)\n",final,maximum);free(final);free(part);}
static void usage(const char *p){fprintf(stderr,"usage: %s [MAX_N] [--memory-mib M] [--no-symmetry]\n       %s --upto MAX_N [options]\n       %s --term N [options]\n       %s --check [MAX_N] [--memory-mib M]\nN is in 1..7; default --upto 6.\n",p,p,p,p);}
int main(int argc,char **argv){Mode mode=MODE_UPTO;int d=-1;bool have_d=false,have_mode=false,symmetry=true,have_memory=false;uint64_t memory=DEFAULT_MEMORY_MIB;for(int i=1;i<argc;++i){const char *s=argv[i];if(!strcmp(s,"--help")||!strcmp(s,"-h")){usage(argv[0]);return 0;}if(!strcmp(s,"--memory-mib")){if(have_memory||++i>=argc){usage(argv[0]);return 1;}memory=parse_memory(argv[i]);have_memory=true;}else if(!strcmp(s,"--no-symmetry"))symmetry=false;else if(!strcmp(s,"--term")||!strcmp(s,"--upto")){if(have_mode||have_d||++i>=argc){usage(argv[0]);return 1;}mode=!strcmp(s,"--term")?MODE_TERM:MODE_UPTO;have_mode=true;d=parse_d(argv[i],"N");have_d=true;}else if(!strcmp(s,"--check")){if(have_mode||have_d){usage(argv[0]);return 1;}mode=MODE_CHECK;have_mode=true;if(i+1<argc&&argv[i+1][0]!='-'){d=parse_d(argv[++i],"CHECK_N");have_d=true;}}else if(s[0]=='-'||have_d){usage(argv[0]);return 1;}else{d=parse_d(s,"MAX_N");have_d=true;}}if(mode==MODE_CHECK)return check_all(have_d?d:DEFAULT_CHECK_D,memory);if(!have_d)d=DEFAULT_MAX_D;if(mode==MODE_TERM){Count x=compute_value(d,symmetry,memory,true);printf("%d ",d);print_count(stdout,x);putchar('\n');return 0;}write_file(argv[0],d,symmetry,memory);return 0;}
