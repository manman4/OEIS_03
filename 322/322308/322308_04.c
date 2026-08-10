/*
 * A322308 -- direct cooldown-CSP search (no subset DP, no IE).
 *
 * Count permutations p of [n] satisfying
 *
 *   |p(i+j)-p(i)| >= 5,       1 <= j <= 4.
 *
 * A prefix is extended only by an unused value compatible with its last four
 * entries.  Thus every leaf, and only every valid permutation, is generated.
 * No state coefficients, memoization, meet-in-the-middle table, or
 * inclusion-exclusion sum is used; this is the A318790_04 search specialized
 * to fixed cooldown 5.
 *
 * Safe capacity lemma.  For C_v={v,...,v+4}, any two members differ by less
 * than 5, so their positions must differ by at least 5.  The current
 * four-entry frontier contains at most one member of C_v.  If q unused
 * members remain and R positions remain, let first=1 when the frontier has
 * no member of C_v.  If its member has age a (newest age 0), let first=5-a.
 * Future occurrence positions t_1<...<t_q satisfy
 *
 *   t_1 >= first,       t_{i+1} >= t_i+5,
 *
 * hence t_q>=first+5*(q-1).  If this exceeds R, completion is impossible.
 * This is a necessary, not sufficient, condition; rejecting only its failure
 * cannot remove a valid permutation.  capacity_possible() implements exactly
 * this inequality immediately after a tentative append.
 *
 * Value complementation c(x)=n-1-x preserves the constraints and has no fixed
 * full permutation for n>=2.  The search keeps p<c(p) lexicographically and
 * doubles its leaf count.  n=0,1 are handled directly.
 *
 * Safety and limits:
 *   - 0<=n<=34; n! bounds the answer and 34!<2^128;
 *   - used sets and all shifts fit unsigned __int128;
 *   - answer arithmetic and 64-bit diagnostic counters are checked;
 *   - each worker stores only one O(n)-length path;
 *   - --check compares capacity pruning on/off through n=10;
 *   - partial output is renamed only after a successful range run.
 * Runtime is exponential and may become large before n=34.
 *
 * Reference: https://oeis.org/A322308
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *     322308_04.c -o 322308_04
 * Usage:
 *   ./322308_04 --term 29 --threads 8
 *   ./322308_04 --upto 29 --threads 8
 *   ./322308_04 --check 10 --threads 8
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
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
#error "322308_04 requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 U128;

#define OFFSET 0
#define MAX_N 34
#define DEFAULT_MAX_N 29
#define DEFAULT_CHECK_N 10
#define MAX_THREADS 64
#define GAP 5
#define WIDTH 4

static const char *const known[30]={
 "1","1","0","0","0","0","0","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","0","0","0","0","0","2","128","6320",
 "344872","22437776"
};
typedef enum {MODE_UPTO,MODE_TERM,MODE_CHECK} Mode;
typedef struct {
    int total,id,workers,failed;
    bool capacity_pruning;
    U128 full,bad[MAX_N],windows[MAX_N];
    U128 solutions;
    uint64_t nodes,pruned;
    char error[128];
} Worker;
typedef struct {uint64_t nodes,pruned;size_t workspace;double seconds;} Stats;

static void die(const char *s){fprintf(stderr,"error: %s\n",s);exit(1);}
static double now_seconds(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))die("clock_gettime failed");return(double)t.tv_sec+(double)t.tv_nsec/1e9;}
static int parse_int(const char *s,const char *label,int lo,int hi){char *e=NULL;errno=0;long x=strtol(s,&e,10);if(errno||e==s||*e||x<lo||x>hi){fprintf(stderr,"error: %s must be in %d..%d: %s\n",label,lo,hi,s);exit(1);}return(int)x;}
static unsigned bit_index(U128 bit){uint64_t low=(uint64_t)bit;return low?(unsigned)__builtin_ctzll(low):64U+(unsigned)__builtin_ctzll((uint64_t)(bit>>64U));}
static unsigned popcount_bits(U128 x){return(unsigned)__builtin_popcountll((uint64_t)x)+(unsigned)__builtin_popcountll((uint64_t)(x>>64U));}
static bool increment_u64(uint64_t *x){if(*x==UINT64_MAX)return false;++*x;return true;}
static bool add_u128(U128 *a,U128 b){U128 old=*a;*a+=b;return *a>=old;}
static bool double_u128(U128 *a){return add_u128(a,*a);}
static int print_u128(FILE *f,U128 x){char b[40];size_t n=0;do{b[n++]=(char)('0'+(unsigned)(x%10U));x/=10U;}while(x);while(n)if(fputc(b[--n],f)==EOF)return-1;return 0;}
static bool parse_u128(const char *s,U128 *result){U128 x=0,max=~(U128)0;if(!*s)return false;for(;*s;++s){if(*s<'0'||*s>'9')return false;unsigned d=(unsigned)(*s-'0');if(x>(max-d)/10U)return false;x=x*10U+d;}*result=x;return true;}

static bool capacity_possible(const Worker *w,U128 used,
                              const unsigned *path,int length)
{
    int remaining_positions=w->total-length;
    int begin=length-WIDTH;if(begin<0)begin=0;
    for(int start=0;start<=w->total-GAP;++start){
        U128 clique=w->windows[start];
        int q=(int)popcount_bits(clique&~used);
        if(q==0)continue;
        int first=1;
        for(int position=length-1;position>=begin;--position)
            if(clique&((U128)1<<path[position])){
                int age=length-1-position;
                first=GAP-age;
                break;
            }
        if(first+GAP*(q-1)>remaining_positions)return false;
    }
    return true;
}

static void search(Worker *w,U128 used,unsigned *path,int length,int relation)
{
    if(!increment_u64(&w->nodes)){w->failed=1;snprintf(w->error,sizeof(w->error),"node counter overflow");return;}
    if(length==w->total){
        if(relation>=0){w->failed=1;snprintf(w->error,sizeof(w->error),"complement canonicalization failure");return;}
        if(!add_u128(&w->solutions,1)){w->failed=1;snprintf(w->error,sizeof(w->error),"answer overflow");}
        return;
    }
    U128 candidates=w->full&~used;
    int begin=length-WIDTH;if(begin<0)begin=0;
    for(int i=begin;i<length;++i)candidates&=~w->bad[path[i]];
    while(candidates&&!w->failed){
        U128 bit=candidates&((U128)0-candidates);candidates^=bit;
        unsigned value=bit_index(bit);
        if(length==0&&(int)(value%(unsigned)w->workers)!=w->id)continue;
        int next_relation=relation;
        if(next_relation==0){unsigned reflected=(unsigned)(w->total-1-(int)value);next_relation=value<reflected?-1:value>reflected?1:0;}
        if(next_relation>0)continue;
        path[length]=value;U128 next_used=used|bit;
        if(w->capacity_pruning&&!capacity_possible(w,next_used,path,length+1)){
            if(!increment_u64(&w->pruned)){w->failed=1;snprintf(w->error,sizeof(w->error),"prune counter overflow");}
            continue;
        }
        search(w,next_used,path,length+1,next_relation);
    }
}

static void *worker_main(void *arg){Worker *w=arg;unsigned path[MAX_N]={0};search(w,0,path,0,0);return NULL;}

static U128 compute(int n,int thread_count,bool capacity,Stats *stats)
{
    double started=now_seconds();memset(stats,0,sizeof(*stats));
    if(n<=1){stats->seconds=now_seconds()-started;return 1;}
    Worker *workers=calloc((size_t)thread_count,sizeof(*workers));
    pthread_t *threads=calloc((size_t)thread_count,sizeof(*threads));
    if(!workers||!threads)die("worker allocation failed");
    for(int id=0;id<thread_count;++id){
        Worker *w=&workers[id];w->total=n;w->id=id;w->workers=thread_count;w->capacity_pruning=capacity;
        w->full=((U128)1<<(unsigned)n)-1U;
        for(int value=0;value<n;++value)for(int other=0;other<n;++other)
            if(abs(value-other)<GAP)w->bad[value]|=(U128)1<<(unsigned)other;
        for(int start=0;start<=n-GAP;++start)for(int value=start;value<start+GAP;++value)
            w->windows[start]|=(U128)1<<(unsigned)value;
        int error=pthread_create(&threads[id],NULL,worker_main,w);
        if(error){fprintf(stderr,"error: pthread_create: %s\n",strerror(error));exit(1);}
    }
    U128 half=0;
    for(int id=0;id<thread_count;++id){int error=pthread_join(threads[id],NULL);if(error){fprintf(stderr,"error: pthread_join: %s\n",strerror(error));exit(1);}Worker *w=&workers[id];if(w->failed)die(w->error);if(!add_u128(&half,w->solutions))die("worker answer overflow");if(UINT64_MAX-stats->nodes<w->nodes||UINT64_MAX-stats->pruned<w->pruned)die("statistics overflow");stats->nodes+=w->nodes;stats->pruned+=w->pruned;}
    stats->workspace=(size_t)thread_count*(sizeof(Worker)+MAX_N*sizeof(unsigned));
    stats->seconds=now_seconds()-started;free(workers);free(threads);
    if(!double_u128(&half))die("oriented answer overflow");return half;
}

static U128 evaluated(int n,int threads,bool verbose)
{
    Stats stats;U128 answer=compute(n,threads,true,&stats);
    if(n<30){U128 expected;if(!parse_u128(known[n],&expected)||answer!=expected){fprintf(stderr,"error: A322308 mismatch at n=%d: got ",n);print_u128(stderr,answer);fprintf(stderr,", expected %s\n",known[n]);exit(1);}}
    if(verbose){fprintf(stderr,"322308_04: n=%d, direct cooldown-CSP, threads=%d, nodes=%"PRIu64", capacity prunes=%"PRIu64", workspace=%.3f MiB, %.3f s\n",n,threads,stats.nodes,stats.pruned,(double)stats.workspace/1048576.0,stats.seconds);}
    return answer;
}

static int check_all(int maximum,int threads)
{
    U128 high=(U128)1<<64U;if(bit_index(high)!=64U||popcount_bits(high|1)!=2U)die("128-bit set self-test failed");
    int plain_max=maximum<10?maximum:10;
    for(int n=OFFSET;n<=maximum;++n){U128 pruned=evaluated(n,threads,false);if(n<=plain_max){Stats stats;U128 plain=compute(n,threads,false,&stats);if(pruned!=plain){fprintf(stderr,"error: capacity/plain mismatch at n=%d\n",n);return 1;}}}
    printf("ok: A322308 known values verified through n=%d; capacity pruning on/off agree through n=%d\n",maximum<29?maximum:29,plain_max);return 0;
}

static char *beside(const char *argv0,const char *name){char exe[PATH_MAX],resolved[PATH_MAX];bool found=false;
#ifdef __APPLE__
uint32_t z=(uint32_t)sizeof(exe);if(_NSGetExecutablePath(exe,&z)==0)found=true;
#elif defined(__linux__)
ssize_t z=readlink("/proc/self/exe",exe,sizeof(exe)-1);if(z>=0){exe[z]='\0';found=true;}
#endif
if(!found){size_t z=strlen(argv0);if(z>=sizeof(exe))die("path too long");memcpy(exe,argv0,z+1);}const char *base=realpath(exe,resolved);if(!base)base=exe;const char *slash=strrchr(base,'/');size_t dir=slash?(size_t)(slash-base):1;if(slash==base)dir=1;size_t len=strlen(name);char *p=malloc(dir+len+2);if(!p)die("path allocation failed");if(!slash)p[0]='.';else if(slash==base)p[0]='/';else memcpy(p,base,dir);p[dir]='/';memcpy(p+dir+1,name,len+1);return p;}
static void write_file(const char *argv0,int maximum,int threads){char *final=beside(argv0,"b322308_04.txt"),*part=beside(argv0,"b322308_04_part.txt");FILE *f=fopen(part,"w");if(!f)die("cannot open partial b-file");for(int n=OFFSET;n<=maximum;++n){U128 x=evaluated(n,threads,true);if(fprintf(f,"%d ",n)<0||print_u128(f,x)<0||fputc('\n',f)==EOF||fflush(f))die("b-file write failed");}if(fclose(f))die("b-file close failed");if(rename(part,final))die("b-file rename failed");printf("wrote %s (n=0..%d)\n",final,maximum);free(final);free(part);}
static void usage(const char *p){fprintf(stderr,"usage: %s [MAX_N] [--threads T] [--memory-mib M]\n       %s --upto MAX_N [options]\n       %s --term N [options]\n       %s --check [MAX_N] [options]\nN is in 0..34. --memory-mib is accepted but unnecessary (O(N) workspace).\n",p,p,p,p);}
int main(int argc,char **argv){Mode mode=MODE_UPTO;int n=-1,threads=1;bool have_mode=false,have_threads=false,have_memory=false;for(int i=1;i<argc;++i){const char *s=argv[i];if(!strcmp(s,"--help")||!strcmp(s,"-h")){usage(argv[0]);return 0;}if(!strcmp(s,"--threads")){if(have_threads||++i>=argc){usage(argv[0]);return 1;}threads=parse_int(argv[i],"THREADS",1,MAX_THREADS);have_threads=true;}else if(!strcmp(s,"--memory-mib")){if(have_memory||++i>=argc){usage(argv[0]);return 1;}(void)parse_int(argv[i],"MEMORY_MIB",1,INT_MAX);have_memory=true;}else if(!strcmp(s,"--term")||!strcmp(s,"--upto")){if(have_mode||++i>=argc){usage(argv[0]);return 1;}mode=!strcmp(s,"--term")?MODE_TERM:MODE_UPTO;n=parse_int(argv[i],"N",OFFSET,MAX_N);have_mode=true;}else if(!strcmp(s,"--check")){if(have_mode){usage(argv[0]);return 1;}mode=MODE_CHECK;have_mode=true;if(i+1<argc&&argv[i+1][0]!='-')n=parse_int(argv[++i],"CHECK_N",OFFSET,MAX_N);}else if(s[0]!='-'&&!have_mode){mode=MODE_UPTO;n=parse_int(s,"MAX_N",OFFSET,MAX_N);have_mode=true;}else{usage(argv[0]);return 1;}}if(n<0)n=mode==MODE_CHECK?DEFAULT_CHECK_N:DEFAULT_MAX_N;if(mode==MODE_CHECK)return check_all(n,threads);if(mode==MODE_TERM){U128 x=evaluated(n,threads,true);printf("%d ",n);print_u128(stdout,x);putchar('\n');return 0;}write_file(argv[0],n,threads);return 0;}
