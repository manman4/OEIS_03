/*
 * A318790 -- direct constraint-search evaluator (no subset DP, no IE).
 *
 * For index d let N=d^2+1 and w=d-1.  We build a permutation from left to
 * right.  A new value is tested only against the preceding w entries, which
 * proves by induction that every generated full permutation satisfies
 *
 *   |p(i+j)-p(i)| >= d,       1 <= j <= w.
 *
 * The important N=d^2+1-specific pruning is a cooldown-capacity test.  For
 * every value interval C_v={v,...,v+d-1}, any two of its members differ by
 * less than d, hence their positions must differ by at least d.  At most one
 * member of C_v can occur in the current w-entry frontier.  If that member
 * has age a (the newest entry has age 0), the earliest future slot available
 * to C_v is d-a; without one it is slot 1.  If q unused members remain, the
 * earliest possible last occurrence is
 *
 *       first + d*(q-1).
 *
 * When this exceeds the number of remaining positions, completion is
 * impossible.  Rejecting exactly this necessary-condition failure is safe;
 * the test is not a heuristic and never removes a valid permutation.
 *
 * Value complementation c(x)=N-1-x is a fixed-point-free involution on full
 * permutations and preserves all constraints.  The search retains exactly
 * the lexicographically smaller of p and c(p).  Therefore its number of
 * leaves is one half of the oriented count, which is A318790(d) (the defining
 * division by two may equivalently use reversal).
 *
 * This algorithm is independent of 318790_01/_02 (used-set coefficient DPs)
 * and 318790_03 (inclusion-exclusion).  It stores only one search path per
 * worker: O(N) memory.  Runtime is exponential, but the cooldown test is very
 * strong for N=d^2+1.  --threads partitions the possible first value.
 *
 * Safety:
 *   - 1<=d<=8, so N<=65 and used sets fit unsigned __int128;
 *   - node, prune, and answer counters are checked for uint64_t overflow;
 *   - all array indices and shifts are bounded by N<=50;
 *   - --check compares pruning on/off through d=3 and known values through
 *     the requested index;
 *   - a partial b-file is renamed only after successful completion.
 *
 * Reference: https://oeis.org/A318790
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *     318790_04.c -o 318790_04
 * Usage:
 *   ./318790_04 --term 7 --threads 8
 *   ./318790_04 --upto 7 --threads 8
 *   ./318790_04 --check 3 --threads 8
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

#define OFFSET 1
#if !defined(__SIZEOF_INT128__)
#error "318790_04 requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 Bits;

#define MAX_D 8
#define DEFAULT_MAX_D 7
#define DEFAULT_CHECK_D 3
#define MAX_THREADS 64

/* n=1..6 are OEIS values; n=7 was independently produced by 01 and 02. */
static const uint64_t known[MAX_D+1]={0,1,7,20,37,64,109,188,0};
typedef enum {MODE_UPTO,MODE_TERM,MODE_CHECK} Mode;

typedef struct {
    int d,total,width,worker_id,worker_count;
    bool capacity_pruning;
    Bits full;
    Bits bad[65];
    Bits value_windows[65];
    uint64_t nodes,pruned,solutions;
    int failed;
    char error[128];
} Worker;

typedef struct {
    uint64_t nodes,pruned;
    size_t memory_bytes;
    double seconds;
} Stats;

static void die(const char *s){fprintf(stderr,"error: %s\n",s);exit(1);}
static double now_seconds(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))die("clock_gettime failed");return(double)t.tv_sec+(double)t.tv_nsec/1e9;}
static int parse_int(const char *s,const char *label,int lo,int hi){char *end=NULL;errno=0;long x=strtol(s,&end,10);if(errno||end==s||*end||x<lo||x>hi){fprintf(stderr,"error: %s must be in %d..%d: %s\n",label,lo,hi,s);exit(1);}return(int)x;}
static unsigned bit_index(Bits bit)
{
    uint64_t low=(uint64_t)bit;
    return low? (unsigned)__builtin_ctzll(low)
              : 64U+(unsigned)__builtin_ctzll((uint64_t)(bit>>64U));
}

static unsigned popcount_bits(Bits value)
{
    return (unsigned)__builtin_popcountll((uint64_t)value)+
           (unsigned)__builtin_popcountll((uint64_t)(value>>64U));
}

static bool checked_increment(uint64_t *x)
{
    if(*x==UINT64_MAX)return false;
    ++*x;return true;
}

/*
 * Test all maximal cliques C_v of the forbidden value-band graph.  Shorter
 * cliques are contained in one of these intervals, so testing all length-d
 * intervals includes the strongest member of this elementary capacity
 * family.  The current valid frontier contains at most one member of C_v.
 */
static bool capacity_possible(const Worker *worker,Bits used,
                              const unsigned *path,int length)
{
    int remaining_positions=worker->total-length;
    int frontier_begin=length-worker->width;
    if(frontier_begin<0)frontier_begin=0;
    for(int start=0;start<=worker->total-worker->d;++start){
        Bits clique=worker->value_windows[start];
        int remaining=(int)popcount_bits(clique&~used);
        if(remaining==0)continue;
        int first=1;
        for(int position=length-1;position>=frontier_begin;--position){
            if(clique&((Bits)1<<path[position])){
                int age=length-1-position;
                first=worker->d-age;
                break;
            }
        }
        int earliest_last=first+worker->d*(remaining-1);
        if(earliest_last>remaining_positions)return false;
    }
    return true;
}

static void search(Worker *worker,Bits used,unsigned *path,int length,
                   int complement_relation)
{
    if(!checked_increment(&worker->nodes)){
        worker->failed=1;
        snprintf(worker->error,sizeof(worker->error),"search-node counter overflow");
        return;
    }
    if(length==worker->total){
        if(complement_relation>=0){
            worker->failed=1;
            snprintf(worker->error,sizeof(worker->error),"complement canonicalization failure");
            return;
        }
        if(!checked_increment(&worker->solutions)){
            worker->failed=1;
            snprintf(worker->error,sizeof(worker->error),"answer counter overflow");
        }
        return;
    }

    Bits candidates=worker->full&~used;
    int begin=length-worker->width;
    if(begin<0)begin=0;
    for(int i=begin;i<length;++i)candidates&=~worker->bad[path[i]];

    while(candidates&&!worker->failed){
        Bits bit=candidates&((Bits)0-candidates);
        candidates^=bit;
        unsigned value=bit_index(bit);
        if(length==0&&(int)(value%(unsigned)worker->worker_count)!=worker->worker_id)
            continue;

        int relation=complement_relation;
        if(relation==0){
            unsigned reflected=(unsigned)(worker->total-1-(int)value);
            relation=value<reflected?-1:value>reflected?1:0;
        }
        if(relation>0)continue; /* keep p < complement(p) only */

        path[length]=value;
        Bits next_used=used|bit;
        if(worker->capacity_pruning&&
           !capacity_possible(worker,next_used,path,length+1)){
            if(!checked_increment(&worker->pruned)){
                worker->failed=1;
                snprintf(worker->error,sizeof(worker->error),"prune counter overflow");
            }
            continue;
        }
        search(worker,next_used,path,length+1,relation);
    }
}

static void *worker_main(void *argument)
{
    Worker *worker=argument;
    unsigned path[65]={0};
    search(worker,0,path,0,0);
    return NULL;
}

static uint64_t compute_search(int d,int thread_count,bool capacity,
                               Stats *stats)
{
    double started=now_seconds();
    memset(stats,0,sizeof(*stats));
    int total=d*d+1;
    Worker *workers=calloc((size_t)thread_count,sizeof(*workers));
    pthread_t *threads=calloc((size_t)thread_count,sizeof(*threads));
    if(!workers||!threads)die("worker allocation failed");

    for(int id=0;id<thread_count;++id){
        Worker *w=&workers[id];
        w->d=d;w->total=total;w->width=d-1;
        w->worker_id=id;w->worker_count=thread_count;
        w->capacity_pruning=capacity;
        w->full=((Bits)1<<(unsigned)total)-1U;
        for(int value=0;value<total;++value)
            for(int other=0;other<total;++other)
                if(abs(value-other)<d)
                    w->bad[value]|=(Bits)1<<(unsigned)other;
        for(int start=0;start<=total-d;++start)
            for(int value=start;value<start+d;++value)
                w->value_windows[start]|=(Bits)1<<(unsigned)value;
        int error=pthread_create(&threads[id],NULL,worker_main,w);
        if(error){fprintf(stderr,"error: pthread_create: %s\n",strerror(error));exit(1);}
    }

    uint64_t answer=0;
    for(int id=0;id<thread_count;++id){
        int error=pthread_join(threads[id],NULL);
        if(error){fprintf(stderr,"error: pthread_join: %s\n",strerror(error));exit(1);}
        Worker *w=&workers[id];
        if(w->failed)die(w->error);
        if(UINT64_MAX-answer<w->solutions)die("answer counter overflow");
        answer+=w->solutions;
        if(UINT64_MAX-stats->nodes<w->nodes||UINT64_MAX-stats->pruned<w->pruned)
            die("statistics counter overflow");
        stats->nodes+=w->nodes;stats->pruned+=w->pruned;
    }
    stats->memory_bytes=(size_t)thread_count*(sizeof(Worker)+65U*sizeof(unsigned));
    stats->seconds=now_seconds()-started;
    free(workers);free(threads);
    return answer;
}

static uint64_t evaluated(int d,int threads,bool verbose)
{
    Stats stats;
    uint64_t answer=compute_search(d,threads,true,&stats);
    if(d<=7&&answer!=known[d]){
        fprintf(stderr,"error: A318790 mismatch at n=%d: got %"PRIu64
                ", expected %"PRIu64"\n",d,answer,known[d]);
        exit(1);
    }
    if(verbose)fprintf(stderr,
        "318790_04: n=%d, N=%d, direct cooldown-CSP, threads=%d, "
        "nodes=%"PRIu64", capacity prunes=%"PRIu64", "
        "workspace=%.3f MiB, %.3f s\n",
        d,d*d+1,threads,stats.nodes,stats.pruned,
        (double)stats.memory_bytes/1048576.0,stats.seconds);
    return answer;
}

static int check_all(int maximum,int threads)
{
    Bits high_bit=(Bits)1<<64U;
    if(bit_index(high_bit)!=64U||popcount_bits(high_bit)!=1U||
       popcount_bits(high_bit|((Bits)1<<3U))!=2U)
        die("128-bit set self-test failed");
    for(int d=OFFSET;d<=maximum;++d){
        uint64_t pruned=evaluated(d,threads,false);
        if(d<=3){
            Stats stats;
            uint64_t plain=compute_search(d,threads,false,&stats);
            if(pruned!=plain){
                fprintf(stderr,"error: capacity/plain search mismatch at n=%d\n",d);
                return 1;
            }
        }
    }
    printf("ok: direct cooldown-CSP computed through n=%d; known values "
           "verified through n=%d; capacity pruning on/off agree through "
           "n=%d\n", maximum,maximum<7?maximum:7,maximum<3?maximum:3);
    return 0;
}

static char *beside(const char *argv0,const char *name)
{
    char exe[PATH_MAX],resolved[PATH_MAX];bool found=false;
#ifdef __APPLE__
    uint32_t size=(uint32_t)sizeof(exe);
    if(_NSGetExecutablePath(exe,&size)==0)found=true;
#elif defined(__linux__)
    ssize_t length=readlink("/proc/self/exe",exe,sizeof(exe)-1U);
    if(length>=0){exe[length]='\0';found=true;}
#endif
    if(!found){size_t n=strlen(argv0);if(n>=sizeof(exe))die("path too long");memcpy(exe,argv0,n+1);}
    const char *base=realpath(exe,resolved);if(!base)base=exe;
    const char *slash=strrchr(base,'/');size_t dir=slash?(size_t)(slash-base):1U;
    if(slash==base)dir=1U;size_t len=strlen(name);
    char *p=malloc(dir+len+2U);if(!p)die("path allocation failed");
    if(!slash)p[0]='.';else if(slash==base)p[0]='/';else memcpy(p,base,dir);
    p[dir]='/';memcpy(p+dir+1U,name,len+1U);return p;
}

static void write_file(const char *argv0,int maximum,int threads)
{
    char *final=beside(argv0,"b318790_04.txt");
    char *part=beside(argv0,"b318790_04_part.txt");
    FILE *f=fopen(part,"w");if(!f)die("cannot open partial b-file");
    for(int d=OFFSET;d<=maximum;++d){uint64_t x=evaluated(d,threads,true);
        if(fprintf(f,"%d %"PRIu64"\n",d,x)<0||fflush(f))die("b-file write failed");}
    if(fclose(f))die("b-file close failed");
    if(rename(part,final))die("b-file rename failed");
    printf("wrote %s (n=1..%d)\n",final,maximum);
    free(final);free(part);
}

static void usage(const char *p)
{
    fprintf(stderr,"usage: %s [MAX_N] [--threads T] [--memory-mib M]\n"
        "       %s --upto MAX_N [--threads T] [--memory-mib M]\n"
        "       %s --term N [--threads T] [--memory-mib M]\n"
        "       %s --check [MAX_N] [--threads T] [--memory-mib M]\n"
        "N is in 1..8; default --upto 7. --memory-mib is accepted for "
        "command compatibility but is unnecessary (workspace is O(N)).\n",
        p,p,p,p);
}

int main(int argc,char **argv)
{
    Mode mode=MODE_UPTO;int d=-1,threads=1;
    bool have_mode=false,have_threads=false,have_memory=false;
    for(int i=1;i<argc;++i){const char *s=argv[i];
        if(!strcmp(s,"--help")||!strcmp(s,"-h")){usage(argv[0]);return 0;}
        if(!strcmp(s,"--threads")){if(have_threads||++i>=argc){usage(argv[0]);return 1;}threads=parse_int(argv[i],"THREADS",1,MAX_THREADS);have_threads=true;}
        else if(!strcmp(s,"--memory-mib")){if(have_memory||++i>=argc){usage(argv[0]);return 1;}(void)parse_int(argv[i],"MEMORY_MIB",1,INT_MAX);have_memory=true;}
        else if(!strcmp(s,"--term")||!strcmp(s,"--upto")){if(have_mode||++i>=argc){usage(argv[0]);return 1;}mode=!strcmp(s,"--term")?MODE_TERM:MODE_UPTO;d=parse_int(argv[i],"N",OFFSET,MAX_D);have_mode=true;}
        else if(!strcmp(s,"--check")){if(have_mode){usage(argv[0]);return 1;}mode=MODE_CHECK;have_mode=true;if(i+1<argc&&argv[i+1][0]!='-')d=parse_int(argv[++i],"CHECK_N",OFFSET,MAX_D);}
        else if(s[0]!='-'&&!have_mode){mode=MODE_UPTO;d=parse_int(s,"MAX_N",OFFSET,MAX_D);have_mode=true;}
        else{usage(argv[0]);return 1;}
    }
    if(d<0)d=mode==MODE_CHECK?DEFAULT_CHECK_D:DEFAULT_MAX_D;
    if(mode==MODE_CHECK)return check_all(d,threads);
    if(mode==MODE_TERM){uint64_t x=evaluated(d,threads,true);printf("%d %"PRIu64"\n",d,x);return 0;}
    write_file(argv[0],d,threads);return 0;
}
