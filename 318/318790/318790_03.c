/*
 * A318790 -- polynomial-space inclusion-exclusion evaluator.
 *
 * For index d let N=d^2+1 and w=d-1.  For S subseteq [N], let W(S) be the
 * number of length-N words over S (repetition allowed) satisfying
 *
 *   |x(i+j)-x(i)| >= d,       1 <= j <= w.
 *
 * Inclusion-exclusion on letters that occur gives the oriented count
 *
 *   P(d) = Sum_{S subseteq [N]} (-1)^(N-|S|) W(S).              (1)
 *
 * A word surviving (1) uses all N letters; since its length is N, it is a
 * required permutation.  A318790(d)=P(d)/2 by reversal.  This method is
 * independent of the used-set prefix DPs in 318790_01.c and _02.c.
 *
 * For fixed S, D_t(q) counts valid words ending in the ordered w-tuple q.
 * Write q=(a,q') and append e.  Then
 *
 *   D_{t+1}(q',e) = Sum_{a in S, |a-e|>=d} D_t(a,q')            (2)
 *
 * when e is compatible with every entry of q'.  The unrestricted sum over
 * a is computed once for each q', then the at most 2d-1 incompatible terms
 * are subtracted.  Renumbering the actual letters of S to 0..|S|-1 makes the
 * workspace O(|S|^w), not O(N^w).
 *
 * Value reflection x->N-1-x preserves W(S), so one representative of each
 * subset orbit is evaluated.  A set not containing d pairwise-d-separated
 * values cannot contain even one valid length-d window and is skipped.
 *
 * Range: 1<=d<=5.  The method examines 2^(d^2+1) subsets, so d=5 is already
 * extremely slow.  On this range each positive/negative half of (1) is at
 * most 2^(N-1)N^N < 2^256.  All 256-bit operations and allocation sizes are
 * checked.  --threads supplies independent subset workers.
 *
 * Reference: https://oeis.org/A318790
 * Inclusion-exclusion background: Kohn, Gottlieb and Kohn, IPL 47 (1993),
 * 203-207, doi:10.1016/0020-0190(93)90033-6.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *     318790_03.c -o 318790_03
 * Usage:
 *   ./318790_03 --term 4 --threads 8
 *   ./318790_03 --upto 4 --threads 8
 *   ./318790_03 --check 3 --threads 8
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
#error "318790_03 requires unsigned __int128"
#endif
__extension__ typedef unsigned __int128 u128;

typedef struct { uint64_t limb[4]; } U256;
#define OFFSET 1
#define MAX_D 5
#define DEFAULT_MAX_D 4
#define DEFAULT_CHECK_D 3
#define DIRECT_MAX_D 3
#define MAX_THREADS 64
static const char *const known[MAX_D+1]={NULL,"1","7","20","37","64"};
typedef enum {MODE_UPTO,MODE_TERM,MODE_CHECK} Mode;
typedef struct {
    int d,total,w,id,workers,failed;
    uint64_t limit,evaluated,skipped,cells;
    U256 positive,negative;
    char error[128];
} Worker;
typedef struct {uint64_t evaluated,skipped,cells;size_t workspace;double seconds;} Stats;

static void die(const char *s){fprintf(stderr,"error: %s\n",s);exit(1);}
static double now_seconds(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))die("clock_gettime failed");return(double)t.tv_sec+(double)t.tv_nsec/1e9;}
static size_t checked_mul(size_t a,size_t b){if(a&&b>SIZE_MAX/a)die("size overflow");return a*b;}
static U256 uzero(void){U256 x={{0,0,0,0}};return x;}
static U256 ufrom(uint64_t x){U256 a={{x,0,0,0}};return a;}
static bool uiszero(U256 a){return(a.limb[0]|a.limb[1]|a.limb[2]|a.limb[3])==0;}
static bool uequal(U256 a,U256 b){return memcmp(&a,&b,sizeof(a))==0;}
static int ucompare(U256 a,U256 b){for(int i=3;i>=0;--i)if(a.limb[i]!=b.limb[i])return a.limb[i]<b.limb[i]?-1:1;return 0;}
static bool uadd(U256 *a,U256 b){uint64_t carry=0;for(int i=0;i<4;++i){u128 s=(u128)a->limb[i]+b.limb[i]+carry;a->limb[i]=(uint64_t)s;carry=(uint64_t)(s>>64);}return carry==0;}
static bool usub(U256 a,U256 b,U256 *r){if(ucompare(a,b)<0)return false;uint64_t borrow=0;for(int i=0;i<4;++i){uint64_t sub=b.limb[i]+borrow,wrapped=sub<b.limb[i];uint64_t next=wrapped||a.limb[i]<sub;r->limb[i]=a.limb[i]-sub;borrow=next;}return borrow==0;}
static bool umulsmall(U256 *a,uint32_t m){uint64_t carry=0;for(int i=0;i<4;++i){u128 p=(u128)a->limb[i]*m+carry;a->limb[i]=(uint64_t)p;carry=(uint64_t)(p>>64);}return carry==0;}
static uint32_t udivsmall(U256 *a,uint32_t d){uint64_t rem=0;for(int i=3;i>=0;--i){u128 x=((u128)rem<<64)|a->limb[i];a->limb[i]=(uint64_t)(x/d);rem=(uint64_t)(x%d);}return(uint32_t)rem;}
static U256 uhalf(U256 a){if(a.limb[0]&1U)die("oriented answer is odd");for(int i=0;i<3;++i)a.limb[i]=(a.limb[i]>>1)|(a.limb[i+1]<<63);a.limb[3]>>=1;return a;}
static int uprint(FILE *f,U256 a){char b[78];size_t n=0;do{b[n++]=(char)('0'+udivsmall(&a,10));}while(!uiszero(a));while(n)if(fputc(b[--n],f)==EOF)return-1;return 0;}
static bool uparse(const char *s,U256 *r){U256 a=uzero();if(!*s)return false;for(;*s;++s){if(*s<'0'||*s>'9'||!umulsmall(&a,10)||!uadd(&a,ufrom((uint64_t)(*s-'0'))))return false;}*r=a;return true;}
static int parse_int(const char *s,const char *label,int lo,int hi){char *e=NULL;errno=0;long x=strtol(s,&e,10);if(errno||e==s||*e||x<lo||x>hi){fprintf(stderr,"error: %s must be in %d..%d: %s\n",label,lo,hi,s);exit(1);}return(int)x;}
static unsigned popcount64(uint64_t x){return(unsigned)__builtin_popcountll(x);}
static uint64_t reflect_mask(uint64_t mask,int total){uint64_t r=0;for(int i=0;i<total;++i)if(mask&(UINT64_C(1)<<(unsigned)i))r|=UINT64_C(1)<<(unsigned)(total-1-i);return r;}
static bool has_d_separated(uint64_t mask,int total,int d){int count=0,last=-d;for(int v=0;v<total;++v)if((mask&(UINT64_C(1)<<(unsigned)v))&&v-last>=d){last=v;if(++count==d)return true;}return false;}

static size_t power_size(size_t base,int exponent){size_t r=1;for(int i=0;i<exponent;++i)r=checked_mul(r,base);return r;}

/* Exact recurrence (2), using base-m encoded frontier tuples. */
static bool word_count(int total,int d,int w,uint64_t mask,
                       U256 *dp,U256 *next,U256 *totals,U256 *answer,
                       uint64_t *cells)
{
    int values[26],m=0;
    for(int v=0;v<total;++v)if(mask&(UINT64_C(1)<<(unsigned)v))values[m++]=v;
    size_t states=power_size((size_t)m,w);
    size_t tails=power_size((size_t)m,w-1);
    memset(dp,0,checked_mul(states,sizeof(*dp)));

    /* The first w positions must be pairwise compatible. */
    for(size_t code=0;code<states;++code){
        size_t q=code;int tuple[4];
        for(int i=w-1;i>=0;--i){tuple[i]=(int)(q%(size_t)m);q/=(size_t)m;}
        bool ok=true;
        for(int i=0;i<w&&ok;++i)for(int j=i+1;j<w;++j)
            if(abs(values[tuple[i]]-values[tuple[j]])<d){ok=false;break;}
        if(ok)dp[code]=ufrom(1);
    }
    for(int length=w;length<total;++length){
        memset(totals,0,checked_mul(tails,sizeof(*totals)));
        memset(next,0,checked_mul(states,sizeof(*next)));
        for(size_t tail=0;tail<tails;++tail)
            for(int a=0;a<m;++a)
                if(!uadd(&totals[tail],dp[(size_t)a*tails+tail]))return false;
        for(size_t tail=0;tail<tails;++tail){
            int tuple[4]={0};size_t q=tail;
            for(int i=w-2;i>=0;--i){tuple[i]=(int)(q%(size_t)m);q/=(size_t)m;}
            for(int e=0;e<m;++e){
                bool ok=true;
                for(int i=0;i<w-1;++i)
                    if(abs(values[tuple[i]]-values[e])<d){ok=false;break;}
                if(!ok)continue;
                U256 value=totals[tail];
                for(int a=0;a<m;++a)if(abs(values[a]-values[e])<d){
                    U256 reduced;
                    if(!usub(value,dp[(size_t)a*tails+tail],&reduced))return false;
                    value=reduced;
                }
                next[tail*(size_t)m+(size_t)e]=value;
                if(*cells==UINT64_MAX)return false;++*cells;
            }
        }
        U256 *tmp=dp;dp=next;next=tmp;
    }
    U256 result=uzero();for(size_t i=0;i<states;++i)if(!uadd(&result,dp[i]))return false;
    *answer=result;return true;
}

static void *worker_main(void *arg)
{
    Worker *w=arg;size_t max_states=power_size((size_t)w->total,w->w);
    size_t max_tails=power_size((size_t)w->total,w->w-1);
    U256 *dp=calloc(max_states,sizeof(*dp)),*next=calloc(max_states,sizeof(*next)),*totals=calloc(max_tails,sizeof(*totals));
    if(!dp||!next||!totals){w->failed=1;snprintf(w->error,sizeof(w->error),"worker workspace allocation failed");free(dp);free(next);free(totals);return NULL;}
    for(uint64_t mask=(uint64_t)w->id;mask<w->limit;mask+=(uint64_t)w->workers){
        uint64_t reflected=reflect_mask(mask,w->total);if(mask>reflected)continue;
        unsigned card=popcount64(mask);
        if(card<(unsigned)w->d||!has_d_separated(mask,w->total,w->d)){++w->skipped;continue;}
        U256 value;
        if(!word_count(w->total,w->d,w->w,mask,dp,next,totals,&value,&w->cells)){w->failed=1;snprintf(w->error,sizeof(w->error),"256-bit recurrence overflow/failure");break;}
        if(mask!=reflected&&!uadd(&value,value)){w->failed=1;snprintf(w->error,sizeof(w->error),"reflection-orbit overflow");break;}
        U256 *destination=((w->total-(int)card)&1)?&w->negative:&w->positive;
        if(!uadd(destination,value)){w->failed=1;snprintf(w->error,sizeof(w->error),"inclusion-exclusion half-sum overflow");break;}
        ++w->evaluated;
    }
    free(dp);free(next);free(totals);return NULL;
}

typedef struct {int d,total,w;bool used[10];int p[10];uint64_t count;} Direct;
static void direct_rec(Direct *c,int length){if(length==c->total){if(c->count==UINT64_MAX)die("direct count overflow");++c->count;return;}for(int v=0;v<c->total;++v)if(!c->used[v]){bool ok=true;for(int j=1;j<=c->w&&j<=length;++j)if(abs(v-c->p[length-j])<c->d){ok=false;break;}if(ok){c->used[v]=true;c->p[length]=v;direct_rec(c,length+1);c->used[v]=false;}}}
static U256 direct_value(int d){Direct c={0};c.d=d;c.total=d*d+1;c.w=d-1;direct_rec(&c,0);if(c.count&1U)die("direct count odd");return ufrom(c.count/2);}

static U256 compute_value(int d,int threads,Stats *stats)
{
    double started=now_seconds();memset(stats,0,sizeof(*stats));
    if(d==1){stats->seconds=now_seconds()-started;return ufrom(1);}
    int total=d*d+1,w=d-1;uint64_t limit=UINT64_C(1)<<(unsigned)total;
    Worker *workers=calloc((size_t)threads,sizeof(*workers));pthread_t *ids=calloc((size_t)threads,sizeof(*ids));if(!workers||!ids)die("worker allocation failed");
    for(int i=0;i<threads;++i){workers[i].d=d;workers[i].total=total;workers[i].w=w;workers[i].id=i;workers[i].workers=threads;workers[i].limit=limit;int e=pthread_create(&ids[i],NULL,worker_main,&workers[i]);if(e){fprintf(stderr,"error: pthread_create: %s\n",strerror(e));exit(1);}}
    U256 positive=uzero(),negative=uzero();
    for(int i=0;i<threads;++i){int e=pthread_join(ids[i],NULL);if(e){fprintf(stderr,"error: pthread_join: %s\n",strerror(e));exit(1);}if(workers[i].failed)die(workers[i].error);if(!uadd(&positive,workers[i].positive)||!uadd(&negative,workers[i].negative))die("worker sum overflow");stats->evaluated+=workers[i].evaluated;stats->skipped+=workers[i].skipped;if(UINT64_MAX-stats->cells<workers[i].cells)die("cell counter overflow");stats->cells+=workers[i].cells;}
    free(workers);free(ids);if(ucompare(positive,negative)<0)die("negative inclusion-exclusion result");U256 oriented;if(!usub(positive,negative,&oriented))die("subtraction failure");
    size_t states=power_size((size_t)total,w),tails=power_size((size_t)total,w-1);
    stats->workspace=checked_mul((size_t)threads,checked_mul(2*states+tails,sizeof(U256)));
    stats->seconds=now_seconds()-started;return uhalf(oriented);
}

static U256 evaluated(int d,int threads,bool verbose){Stats s;U256 x=compute_value(d,threads,&s),expected;if(!uparse(known[d],&expected)||!uequal(x,expected)){fprintf(stderr,"error: A318790 mismatch at n=%d: got ",d);uprint(stderr,x);fprintf(stderr,", expected %s\n",known[d]);exit(1);}if(verbose)fprintf(stderr,"318790_03: n=%d, N=%d, inclusion-exclusion, threads=%d, evaluated/skipped reflection representatives=%"PRIu64"/%"PRIu64", recurrence cells=%"PRIu64", workspace=%.2f MiB, %.3f s\n",d,d*d+1,threads,s.evaluated,s.skipped,s.cells,(double)s.workspace/1048576.0,s.seconds);return x;}
static int check_all(int maximum,int threads){/* Exercise carries beyond 128 bits. */U256 a={{UINT64_MAX,UINT64_MAX,1,0}},b=a,expected={{0,0,2,0}},recovered;if(!uadd(&b,ufrom(1))||!uequal(b,expected)||!usub(b,ufrom(1),&recovered)||!uequal(recovered,a))die("U256 self-test failed");for(int d=1;d<=maximum;++d){U256 x=evaluated(d,threads,false);if(d<=DIRECT_MAX_D&&!uequal(x,direct_value(d)))die("inclusion-exclusion/direct mismatch");}printf("ok: independent inclusion-exclusion agrees with A318790 through n=%d and direct backtracking through n=%d\n",maximum,maximum<DIRECT_MAX_D?maximum:DIRECT_MAX_D);return 0;}

static char *beside(const char *argv0,const char *name){char exe[PATH_MAX],resolved[PATH_MAX];bool found=false;
#ifdef __APPLE__
uint32_t z=(uint32_t)sizeof(exe);if(_NSGetExecutablePath(exe,&z)==0)found=true;
#elif defined(__linux__)
ssize_t z=readlink("/proc/self/exe",exe,sizeof(exe)-1);if(z>=0){exe[z]='\0';found=true;}
#endif
if(!found){size_t z=strlen(argv0);if(z>=sizeof(exe))die("path too long");memcpy(exe,argv0,z+1);}const char *base=realpath(exe,resolved);if(!base)base=exe;const char *slash=strrchr(base,'/');size_t dir=slash?(size_t)(slash-base):1;if(slash==base)dir=1;size_t len=strlen(name);char *p=malloc(dir+len+2);if(!p)die("path allocation failed");if(!slash)p[0]='.';else if(slash==base)p[0]='/';else memcpy(p,base,dir);p[dir]='/';memcpy(p+dir+1,name,len+1);return p;}
static void write_file(const char *argv0,int maximum,int threads){char *final=beside(argv0,"b318790_03.txt"),*part=beside(argv0,"b318790_03_part.txt");FILE *f=fopen(part,"w");if(!f)die("cannot open partial b-file");for(int d=OFFSET;d<=maximum;++d){U256 x=evaluated(d,threads,true);if(fprintf(f,"%d ",d)<0||uprint(f,x)<0||fputc('\n',f)==EOF||fflush(f))die("b-file write failed");}if(fclose(f))die("b-file close failed");if(rename(part,final))die("b-file rename failed");printf("wrote %s (n=1..%d)\n",final,maximum);free(final);free(part);}
static void usage(const char *p){fprintf(stderr,"usage: %s [MAX_N] [--threads T]\n       %s --upto MAX_N [--threads T]\n       %s --term N [--threads T]\n       %s --check [MAX_N] [--threads T]\nN is in 1..5 (default 4); n=5 is extremely slow.\n",p,p,p,p);}
int main(int argc,char **argv){Mode mode=MODE_UPTO;int d=-1,threads=1;bool have_mode=false,have_threads=false;for(int i=1;i<argc;++i){const char *s=argv[i];if(!strcmp(s,"--help")||!strcmp(s,"-h")){usage(argv[0]);return 0;}if(!strcmp(s,"--threads")){if(have_threads||++i>=argc){usage(argv[0]);return 1;}threads=parse_int(argv[i],"THREADS",1,MAX_THREADS);have_threads=true;}else if(!strcmp(s,"--term")||!strcmp(s,"--upto")){if(have_mode||++i>=argc){usage(argv[0]);return 1;}mode=!strcmp(s,"--term")?MODE_TERM:MODE_UPTO;d=parse_int(argv[i],"N",OFFSET,MAX_D);have_mode=true;}else if(!strcmp(s,"--check")){if(have_mode){usage(argv[0]);return 1;}mode=MODE_CHECK;have_mode=true;if(i+1<argc&&argv[i+1][0]!='-')d=parse_int(argv[++i],"CHECK_N",OFFSET,MAX_D);}else if(s[0]!='-'&&!have_mode){mode=MODE_UPTO;d=parse_int(s,"MAX_N",OFFSET,MAX_D);have_mode=true;}else{usage(argv[0]);return 1;}}if(d<0)d=mode==MODE_CHECK?DEFAULT_CHECK_D:DEFAULT_MAX_D;if(mode==MODE_CHECK)return check_all(d,threads);if(mode==MODE_TERM){U256 x=evaluated(d,threads,true);printf("%d ",d);uprint(stdout,x);putchar('\n');return 0;}write_file(argv[0],d,threads);return 0;}
