/*
 * A306785: primes p with A050443(p) == 0 (mod p^2).
 * A050443(0..3) = 4,0,0,3; a(n) = a(n-3) + a(n-4).
 * Definition: https://oeis.org/A306785 and https://oeis.org/A050443
 *
 * Build and run from the directory containing this file (GCC or Clang):
 *   cc -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *      306785_01.c -o 306785_01
 *   ./306785_01 --self-test
 *   ./306785_01 2 100000000000 8 > 306785_1e11.txt
 *   ./306785_01 100000000001 1000000000000 8 > 306785_extension.txt
 *   ./306785_01 999900000000 1000000000000 8
 *   ./306785_01 --check 1051
 *
 * Usage: START END [THREADS [SEGMENT_ODDS]], both endpoints INCLUSIVE.
 * Maximum END: 10^12. Default THREADS: online CPUs (at most 256).
 * Default SEGMENT_ODDS: 1048576 (one byte per odd integer per worker).
 * No GMP, OpenMP, prime files, b-files, or floating point arithmetic in the
 * search. POSIX threads and compiler support for unsigned __int128 required.
 * stdout: matching primes, one per line, in increasing order.
 * stderr: progress, prime count, elapsed time, and completed interval.
 * Ctrl-C/SIGTERM: finish already assigned segments, flush ordered results,
 * report the fully completed prefix and an exact resume command, exit 130/143.
 * Resume into a NEW output file; combine the two files afterwards.
 * A failed output write causes a nonzero exit; do not treat it as completion.
 *
 * Mathematical reduction (valid for all indices, not just primes):
 * In Z[x]/(x^4-x-1), write x^n = c0+c1*x+c2*x^2+c3*x^3. Taking the sum
 * over the four complex roots gives a(n) = 4*c0+3*c3. Both sides satisfy
 * the defining recurrence and have the same first four values. Therefore
 * reduction modulo p^2 and testing this trace is exact.
 *
 * Left-to-right binary exponentiation needs ONLY squaring and multiplication
 * by x. For f=a+b*x+c*x^2+d*x^3, reducing f^2 gives
 *   r0 = a*a + c*c + 2*b*d
 *   r1 = 2*a*b + c*c + 2*b*d + 2*c*d
 *   r2 = b*b + 2*a*c + 2*c*d + d*d
 *   r3 = 2*a*d + 2*b*c + d*d.
 * Multiplication by x maps r to (r3,r0+r3,r1,r2).
 * Since every tested p except 2 is odd, the FINAL step needs only
 *   trace(x*f^2) = 8*a*d + 8*b*c + 7*d*d + 3*b*b + 6*a*c + 6*c*d.
 * This saves the final four-coefficient square.
 * The leading four exponent bits are initialized from the exact x^8..x^15
 * coefficient table below, avoiding the first three squarings.
 *
 * Arithmetic bounds:
 *  - p <= 10^12 implies m=p^2 < 2^80, and all stored coefficients < m.
 *  - For m < 2^61 use radix R=2^64. A square accumulator is <7*m^2,
 *    so its Montgomery reduction is <2*m, and the addition fits 128 bits.
 *    The final trace accumulator is <38*m^2; its reduction is <6*m.
 *  - Otherwise use R=2^128. Products and sums are stored in THREE 64-bit
 *    limbs (192 bits); even the final trace is <38*2^160 <2^166.
 *    Both square and trace accumulators are <m*R, hence REDC <2*m.
 *    The two cancellation steps use 128-bit partial products; no 160-bit
 *    product is truncated to __int128. The high limbs of coefficients
 *    are <2^16, so the cross-product sums also fit in 128 bits.
 *  - m is odd. R is invertible modulo m, so a Montgomery trace is zero
 *    exactly when the ordinary trace is zero.
 *
 * Exhaustiveness: sieve base primes extend through floor(sqrt(END)). Every
 * odd composite in a segment has a prime factor no larger than that bound.
 * Marking starts at max(q^2, first odd multiple of q in the segment), so no
 * prime is removed. There are no congruence filters or assumed known terms.
 *
 * Self-test: direct defining recurrence on small odd indices; independent
 * 4x4 matrix exponentiation with binary-addition modular multiplication at
 * 32/64-bit boundaries and near 10^11 and 10^12; independent Montgomery
 * multiplication checks. Search/sieve integration can be checked by
 * ./306785_01 2 1000000: stderr must report 78498 primes tested.
 */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if defined(__APPLE__)
extern int sysctlbyname(const char *, void *, size_t *, void *, size_t);
#endif

#ifndef __SIZEOF_INT128__
#error "This program requires unsigned __int128 (GCC or Clang)."
#endif
__extension__ typedef unsigned __int128 u128;

#define MAX_END UINT64_C(1000000000000)
#define DEFAULT_SEGMENT UINT64_C(1048576)
#define MAX_THREADS 256U
#define FAST_LIMIT UINT64_C(1518500249) /* floor(sqrt(2^61-1)) */

typedef struct { uint64_t lo, mid, hi; } U192;
typedef struct { u128 m, one; uint64_t m0, m1, inv; } Mont;

/* A handler must not acquire a hidden atomic-library lock. The flag carries
 * no other shared state, so relaxed atomic loads/stores are sufficient. */
_Static_assert(ATOMIC_INT_LOCK_FREE == 2,
               "Signal handling requires always-lock-free atomic_int");
static atomic_int caught_signal = ATOMIC_VAR_INIT(0);

static void on_signal(int number)
{
    atomic_store_explicit(&caught_signal,number,memory_order_relaxed);
}

static inline int received_signal(void)
{
    return atomic_load_explicit(&caught_signal,memory_order_relaxed);
}

static void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static uint64_t parse_u64(const char *s)
{
    if (!s[0]) die("empty integer");
    for (const char *p = s; *p; ++p)
        if (*p < '0' || *p > '9') die("use unsigned decimal integers");
    errno = 0;
    char *end;
    unsigned long long n = strtoull(s, &end, 10);
    if (errno || *end) die("integer out of range");
    return (uint64_t)n;
}

static double seconds(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t)) die("clock_gettime failed");
    return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
}

static void print_u128(FILE *out, u128 n)
{
    char b[40];
    size_t k = 0;
    do { b[k++] = (char)('0' + n % 10); n /= 10; } while (n);
    while (k) fputc(b[--k], out);
}

static inline u128 add_mod(u128 a, u128 b, u128 m)
{
    u128 t = a + b; /* <2^81 */
    return t >= m ? t - m : t;
}

static Mont make_mont(uint64_t p, bool wide)
{
    Mont c;
    c.m = (u128)p * p;
    c.m0 = (uint64_t)c.m;
    c.m1 = (uint64_t)(c.m >> 64);
    uint64_t v = 1;
    for (unsigned i = 0; i < 6; ++i) v *= 2 - c.m0 * v;
    c.inv = 0 - v;
    if (wide) {
        c.one = (~(u128)0) % c.m + 1; /* 2^128 mod m */
        if (c.one == c.m) c.one = 0;
    } else {
        c.one = ((u128)1 << 64) % c.m;
    }
    return c;
}

static inline uint64_t reduce64(u128 t, const Mont *c)
{
    uint64_t q = (uint64_t)t * c->inv;
    uint64_t r = (uint64_t)((t + (u128)q * c->m0) >> 64);
    return r >= c->m0 ? r - c->m0 : r;
}

static inline U192 product128(u128 a, u128 b)
{
    uint64_t a0 = (uint64_t)a, a1 = (uint64_t)(a >> 64);
    uint64_t b0 = (uint64_t)b, b1 = (uint64_t)(b >> 64);
    u128 low = (u128)a0 * b0;
    u128 cross = (u128)a0 * b1 + (u128)a1 * b0 + (low >> 64);
    U192 r = {(uint64_t)low, (uint64_t)cross,
              a1 * b1 + (uint64_t)(cross >> 64)};
    return r;
}

static inline U192 add192(U192 a, U192 b)
{
    u128 low = (u128)a.lo + b.lo;
    u128 mid = (u128)a.mid + b.mid + (uint64_t)(low >> 64);
    U192 r = {(uint64_t)low, (uint64_t)mid,
              a.hi + b.hi + (uint64_t)(mid >> 64)};
    return r;
}

static inline U192 twice192(U192 a)
{
    U192 r = {a.lo << 1, (a.mid << 1) | (a.lo >> 63),
              (a.hi << 1) | (a.mid >> 63)};
    return r;
}

static inline U192 scale192(U192 a, unsigned k)
{
    u128 lo = (u128)a.lo * k;
    u128 mid = (u128)a.mid * k + (uint64_t)(lo >> 64);
    U192 r = {(uint64_t)lo, (uint64_t)mid,
              a.hi * k + (uint64_t)(mid >> 64)};
    return r;
}

/* REDC of a 192-bit T < m*2^128, without ever constructing a 256-bit T. */
static inline u128 reduce128(U192 t, const Mont *c)
{
    uint64_t q = t.lo * c->inv;
    u128 low = (u128)q * c->m0;
    /* t.lo+low.lo is 0 mod 2^64; its carry is (t.lo != 0). */
    u128 s = (u128)q * c->m1 + (low >> 64) + t.mid + (t.lo != 0);
    uint64_t high = t.hi + (uint64_t)(s >> 64);
    uint64_t mid = (uint64_t)s;
    q = mid * c->inv;
    low = (u128)q * c->m0;
    s = (u128)q * c->m1 + (low >> 64) + high + (mid != 0);
    return s >= c->m ? s - c->m : s;
}

static inline void square64(uint64_t f[4], const Mont *m, bool bit)
{
    uint64_t a=f[0], b=f[1], c=f[2], d=f[3];
    u128 aa=(u128)a*a, bb=(u128)b*b, cc=(u128)c*c, dd=(u128)d*d;
    u128 ab=2*(u128)a*b, ac=2*(u128)a*c, ad=2*(u128)a*d;
    u128 bc=2*(u128)b*c, bd=2*(u128)b*d, cd=2*(u128)c*d;
    uint64_t r0=reduce64(aa+cc+bd,m), r1=reduce64(ab+cc+bd+cd,m);
    uint64_t r2=reduce64(bb+ac+cd+dd,m), r3=reduce64(ad+bc+dd,m);
    if (bit) {
        uint64_t sum = r0+r3;
        f[0]=r3; f[1]=sum >= m->m0 ? sum-m->m0 : sum;
        f[2]=r1; f[3]=r2;
    } else {
        f[0]=r0; f[1]=r1; f[2]=r2; f[3]=r3;
    }
}

static inline void square128(u128 f[4], const Mont *m, bool bit)
{
    u128 a=f[0], b=f[1], c=f[2], d=f[3];
    U192 aa=product128(a,a), bb=product128(b,b);
    U192 cc=product128(c,c), dd=product128(d,d);
    U192 ab=twice192(product128(a,b)), ac=twice192(product128(a,c));
    U192 ad=twice192(product128(a,d)), bc=twice192(product128(b,c));
    U192 bd=twice192(product128(b,d)), cd=twice192(product128(c,d));
    U192 v=add192(cc,bd), w=add192(cd,dd);
    u128 r0=reduce128(add192(aa,v),m);
    u128 r1=reduce128(add192(add192(ab,v),cd),m);
    u128 r2=reduce128(add192(add192(bb,ac),w),m);
    u128 r3=reduce128(add192(add192(ad,bc),dd),m);
    if (bit) {
        f[0]=r3; f[1]=add_mod(r0,r3,m->m); f[2]=r1; f[3]=r2;
    } else {
        f[0]=r0; f[1]=r1; f[2]=r2; f[3]=r3;
    }
}

/* Returns a(n) * R mod n^2. n must be odd, 3 <= n <= MAX_END. */
static u128 trace_mont(uint64_t n, Mont *m, bool wide)
{
    *m = make_mont(n,wide);
    unsigned bit = (63U - (unsigned)__builtin_clzll(n)) & 63U;
    static const uint8_t leading[8][4]={
        {1,2,1,0},{0,1,2,1},{1,1,1,2},{2,3,1,1},
        {1,3,3,1},{1,2,3,3},{3,4,2,3},{3,6,4,2}
    };
    const uint8_t *prefix=NULL;
    if (bit>=4) {
        prefix=leading[(n>>(bit-3))&7];
        bit-=4;
    } else if (bit) --bit;
    if (!wide) {
        uint64_t f[4]={0,(uint64_t)m->one,0,0};
        if (prefix) {
            uint64_t v[7]={0,(uint64_t)m->one};
            for (unsigned k=2;k<=6;++k) {
                uint64_t sum=v[k-1]+v[1];
                v[k]=sum>=m->m0 ? sum-m->m0 : sum;
            }
            for (unsigned k=0;k<4;++k) f[k]=v[prefix[k]];
        }
        for (; bit >= 1; --bit) square64(f,m,((n>>bit)&1)!=0);
        uint64_t a=f[0], b=f[1], c=f[2], d=f[3];
        u128 t=8*(u128)a*d+8*(u128)b*c+7*(u128)d*d+
               3*(u128)b*b+6*(u128)a*c+6*(u128)c*d;
        uint64_t q=(uint64_t)t*m->inv;
        uint64_t r=(uint64_t)((t+(u128)q*m->m0)>>64);
        while (r >= m->m0) r -= m->m0;
        return r;
    }
    u128 f[4]={0,m->one,0,0};
    if (prefix) {
        u128 v[7]={0,m->one};
        for (unsigned k=2;k<=6;++k) v[k]=add_mod(v[k-1],v[1],m->m);
        for (unsigned k=0;k<4;++k) f[k]=v[prefix[k]];
    }
    for (; bit >= 1; --bit) square128(f,m,((n>>bit)&1)!=0);
    u128 a=f[0], b=f[1], c=f[2], d=f[3];
    U192 t=add192(scale192(product128(a,d),8),scale192(product128(b,c),8));
    t=add192(t,scale192(product128(d,d),7));
    t=add192(t,scale192(product128(b,b),3));
    t=add192(t,scale192(product128(a,c),6));
    t=add192(t,scale192(product128(c,d),6));
    return reduce128(t,m);
}

static bool is_term(uint64_t p)
{
    if (p == 2) return true; /* a(2)=0, directly from the definition. */
    Mont m;
    return trace_mont(p,&m,p>FAST_LIMIT) == 0;
}

static u128 residue(uint64_t p, bool wide)
{
    if (p == 2) return 0;
    Mont m;
    u128 t=trace_mont(p,&m,wide);
    if (wide) {
        U192 v={(uint64_t)t,(uint64_t)(t>>64),0};
        return reduce128(v,&m);
    }
    return reduce64(t,&m);
}

/* Independent oracle: ordinary residues, add-and-double multiplication,
 * 4x4 companion matrices; does not share polynomial or Montgomery code. */
static u128 reference_mul(u128 a, u128 b, u128 m)
{
    u128 r=0;
    while (b) {
        if (b&1) { r+=a; if (r>=m) r-=m; }
        b>>=1;
        a+=a; if (a>=m) a-=m;
    }
    return r;
}

typedef struct { u128 a[4][4]; } Matrix;

static Matrix matrix_mul(const Matrix *a, const Matrix *b, u128 m)
{
    Matrix c={{{0}}};
    for (unsigned i=0;i<4;++i) for (unsigned j=0;j<4;++j)
        for (unsigned k=0;k<4;++k) {
            c.a[i][j]+=reference_mul(a->a[i][k],b->a[k][j],m);
            if (c.a[i][j]>=m) c.a[i][j]-=m;
        }
    return c;
}

static u128 reference_trace(uint64_t n)
{
    u128 m=(u128)n*n;
    Matrix a={{{0,1,0,0},{0,0,1,0},{0,0,0,1},{1,1,0,0}}};
    Matrix r={{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}};
    uint64_t e=n;
    while (e) {
        if (e&1) r=matrix_mul(&r,&a,m);
        e>>=1;
        if (e) a=matrix_mul(&a,&a,m);
    }
    u128 t=0;
    for (unsigned i=0;i<4;++i) { t+=r.a[i][i]; if (t>=m) t-=m; }
    return t;
}

static uint64_t rng_state=UINT64_C(0x8d4177ce5361b0ad);
static uint64_t random64(void)
{
    rng_state^=rng_state<<13; rng_state^=rng_state>>7; rng_state^=rng_state<<17;
    return rng_state;
}

static u128 random_residue(u128 m)
{
    return (((u128)random64()<<64)|random64())%m;
}

static void self_test(void)
{
    unsigned small=0, matrices=0, arithmetic=0;
    for (uint64_t n=3;n<=2001;n+=2) {
        u128 m=(u128)n*n, f[4]={4,0,0,3};
        for (uint64_t i=4;i<=n;++i) f[i%4]=(f[i%4]+f[(i-3)%4])%m;
        u128 expected=f[n%4]%m;
        if (residue(n,false)!=expected || residue(n,true)!=expected)
            die("self-test: direct recurrence mismatch");
        ++small;
    }
    const uint64_t edges[]={3,5,1051,65535,65537,4294967295,4294967297,
        FAST_LIMIT-2,FAST_LIMIT,FAST_LIMIT+2,99999999999,100000000001,
        999999999999,MAX_END-3};
    for (unsigned i=0;i<sizeof edges/sizeof edges[0]+48;++i) {
        uint64_t n=i<sizeof edges/sizeof edges[0] ? edges[i] :
                   3+2*(random64()%((MAX_END-3)/2));
        u128 expected=reference_trace(n);
        if (residue(n,n>FAST_LIMIT)!=expected || residue(n,true)!=expected)
            die("self-test: independent matrix mismatch");
        ++matrices;
    }
    for (unsigned i=0;i<3000;++i) {
        uint64_t n=3+2*(random64()%((MAX_END-3)/2));
        Mont m=make_mont(n,true);
        u128 a=random_residue(m.m), b=random_residue(m.m);
        if (i%4==0) a=m.m-1;
        if (i%5==0) b=m.m-1;
        u128 ar=reference_mul(a,m.one,m.m), br=reference_mul(b,m.one,m.m);
        u128 c=reduce128(product128(ar,br),&m);
        U192 v={(uint64_t)c,(uint64_t)(c>>64),0};
        if (reduce128(v,&m)!=reference_mul(a,b,m.m))
            die("self-test: 128-bit Montgomery multiplication mismatch");
        if (reduce128(scale192(product128(ar,br),38),&m)!=
            reference_mul(reference_mul(a,b,m.m),38*m.one % m.m,m.m))
            die("self-test: accumulated product mismatch");
        n=3+2*(random64()%((FAST_LIMIT-3)/2));
        m=make_mont(n,false);
        a=i%2 ? m.m-1 : random_residue(m.m);
        b=i%3 ? m.m-1 : random_residue(m.m);
        ar=reference_mul(a,m.one,m.m); br=reference_mul(b,m.one,m.m);
        c=reduce64(ar*br,&m);
        if (reduce64(c,&m)!=reference_mul(a,b,m.m))
            die("self-test: 64-bit Montgomery multiplication mismatch");
        ++arithmetic;
    }
    fprintf(stderr,"self-test PASS: %u direct recurrence indices (both backends), "
            "%u independent matrices, %u arithmetic pairs (both backends)\n",
            small,matrices,arithmetic);
}

typedef struct { uint32_t *p; size_t count; } PrimeTable;

static PrimeTable make_primes(uint64_t end)
{
    uint32_t root=0;
    /* Integer sqrt by binary search; no floating point boundary assumption. */
    uint32_t lo=0, hi=1000001;
    while (lo+1<hi) {
        uint32_t mid=lo+(hi-lo)/2;
        if ((uint64_t)mid*mid<=end) lo=mid; else hi=mid;
    }
    root=lo;
    uint8_t *composite=calloc((size_t)root+1,1);
    uint32_t *primes=malloc(((size_t)root+1)*sizeof *primes);
    if (!composite || !primes) die("base prime allocation failed");
    size_t count=0;
    for (uint32_t p=2;p<=root;++p) if (!composite[p]) {
        primes[count++]=p;
        if ((uint64_t)p*p<=root)
            for (uint32_t k=p*p;k<=root;k+=p) composite[k]=1;
    }
    free(composite);
    PrimeTable t={primes,count};
    return t;
}

typedef struct { uint64_t *p; size_t count, capacity; bool done; } Result;
typedef struct {
    uint64_t start,end,first,odds,segment,segments;
    uint64_t issued,published,completed,primes;
    unsigned active,window;
    bool failed;
    PrimeTable base;
    Result *slots;
    pthread_mutex_t lock;
    pthread_cond_t changed;
} Job;

static bool save_hit(Result *r, uint64_t p)
{
    if (r->count==r->capacity) {
        size_t cap=r->capacity ? 2*r->capacity : 4;
        uint64_t *v=realloc(r->p,cap*sizeof *v);
        if (!v) return false;
        r->p=v; r->capacity=cap;
    }
    r->p[r->count++]=p;
    return true;
}

static void *worker(void *arg)
{
    Job *j=arg;
    uint8_t *sieve=malloc((size_t)j->segment);
    if (!sieve) {
        pthread_mutex_lock(&j->lock);
        j->failed=true;
        pthread_mutex_unlock(&j->lock);
    }
    while (sieve) {
        pthread_mutex_lock(&j->lock);
        while (j->issued<j->segments && j->issued-j->published==j->window &&
               !received_signal() && !j->failed)
            pthread_cond_wait(&j->changed,&j->lock);
        if (received_signal() || j->failed || j->issued==j->segments) {
            pthread_mutex_unlock(&j->lock); break;
        }
        uint64_t index=j->issued++;
        pthread_mutex_unlock(&j->lock);
        uint64_t offset=index*j->segment;
        uint64_t count=j->odds-offset;
        if (count>j->segment) count=j->segment;
        uint64_t first=j->first+2*offset, last=first+2*(count-1);
        memset(sieve,0,(size_t)count);
        for (size_t k=0;k<j->base.count;++k) {
            uint64_t q=j->base.p[k];
            if (q==2) continue;
            if (q*q>last) break;
            uint64_t v=((first+q-1)/q)*q;
            if (v<q*q) v=q*q;
            if (!(v&1)) v+=q;
            for (uint64_t pos=(v-first)/2;pos<count;pos+=q) sieve[pos]=1;
        }
        Result r={0};
        uint64_t tested=0;
        bool ok=true;
        for (uint64_t k=0;k<count;++k) if (!sieve[k]) {
            uint64_t p=first+2*k;
            ++tested;
            if (is_term(p) && !save_hit(&r,p)) { ok=false; break; }
        }
        pthread_mutex_lock(&j->lock);
        if (!ok) {
            free(r.p); j->failed=true;
        } else {
            r.done=true;
            j->slots[index%j->window]=r;
            j->primes+=tested;
            ++j->completed;
            bool wrote=false;
            while (j->published<j->issued) {
                Result *s=&j->slots[j->published%j->window];
                if (!s->done) break;
                for (size_t k=0;k<s->count;++k) {
                    if (printf("%" PRIu64 "\n",s->p[k])<0) j->failed=true;
                    wrote=true;
                }
                free(s->p); memset(s,0,sizeof *s);
                ++j->published;
            }
            if (wrote && fflush(stdout)) j->failed=true;
        }
        pthread_cond_broadcast(&j->changed);
        pthread_mutex_unlock(&j->lock);
    }
    free(sieve);
    pthread_mutex_lock(&j->lock);
    --j->active;
    pthread_cond_broadcast(&j->changed);
    pthread_mutex_unlock(&j->lock);
    return NULL;
}

static unsigned default_threads(void)
{
    long n=1;
#if defined(__APPLE__)
    int cpus=1;
    size_t size=sizeof cpus;
    if (!sysctlbyname("hw.logicalcpu",&cpus,&size,NULL,0)) n=cpus;
#elif defined(_SC_NPROCESSORS_ONLN)
    n=sysconf(_SC_NPROCESSORS_ONLN);
#endif
    return n<1 ? 1U : n>MAX_THREADS ? MAX_THREADS : (unsigned)n;
}

static void usage(FILE *out)
{
    fputs("Usage: ./306785_01 START END [THREADS [SEGMENT_ODDS]]\n"
          "       ./306785_01 --check P\n"
          "       ./306785_01 --self-test\n"
          "Endpoints inclusive; END <= 1000000000000; exact decimal integers.\n"
          "Matching primes on stdout; progress and completion on stderr.\n",out);
}

int main(int argc, char **argv)
{
    if (argc==2 && !strcmp(argv[1],"--self-test")) { self_test(); return 0; }
    if (argc==2 && !strcmp(argv[1],"--help")) { usage(stdout); return 0; }
    if (argc==3 && !strcmp(argv[1],"--check")) {
        uint64_t p=parse_u64(argv[2]);
        if (p<2 || p>MAX_END) die("P must be between 2 and 10^12");
        PrimeTable base=make_primes(p);
        for (size_t i=0;i<base.count;++i) if (p%base.p[i]==0 && p!=base.p[i])
            die("P is composite");
        free(base.p);
        u128 r=residue(p,p>FAST_LIMIT);
        printf("p=%" PRIu64 " A050443(p) mod p^2=",p);
        print_u128(stdout,r);
        printf(" member=%s\n",r==0 ? "yes" : "no");
        return fflush(stdout) ? EXIT_FAILURE : 0;
    }
    if (argc<3 || argc>5) { usage(stderr); return EXIT_FAILURE; }
    uint64_t start=parse_u64(argv[1]), end=parse_u64(argv[2]);
    if (start>end || end>MAX_END) die("require START <= END <= 10^12");
    uint64_t nt=argc>=4 ? parse_u64(argv[3]) : default_threads();
    uint64_t seg=argc>=5 ? parse_u64(argv[4]) : DEFAULT_SEGMENT;
    if (nt<1 || nt>MAX_THREADS) die("THREADS must be in 1..256");
    if (seg<1024 || seg>UINT64_C(16777216))
        die("SEGMENT_ODDS must be in 1024..16777216");
    Job j={0};
    j.start=start; j.end=end; j.segment=seg;
    j.first=start<3 ? 3 : start|UINT64_C(1);
    if (j.first<=end) j.odds=(end-j.first)/2+1;
    j.segments=(j.odds+seg-1)/seg;
    unsigned threads=(unsigned)nt;
    if (j.segments<threads) threads=(unsigned)j.segments;
    j.window=threads ? 2*threads : 1;
    j.slots=calloc(j.window,sizeof *j.slots);
    if (!j.slots) die("result allocation failed");
    if (pthread_mutex_init(&j.lock,NULL) || pthread_cond_init(&j.changed,NULL))
        die("pthread initialization failed");
    j.base=make_primes(end);
    struct sigaction sa;
    memset(&sa,0,sizeof sa); sa.sa_handler=on_signal; sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT,&sa,NULL) || sigaction(SIGTERM,&sa,NULL))
        die("sigaction failed");
    /* Treat closed output pipes as write failures, not successful searches. */
    signal(SIGPIPE,SIG_IGN);
    if (start<=2 && end>=2) {
        if (puts("2")==EOF || fflush(stdout)) die("stdout write failed");
        j.primes=1;
    }
    double begin=seconds(), last_report=begin;
    fprintf(stderr,"A306785 search [%" PRIu64 ", %" PRIu64 "] inclusive; "
            "%u threads; %" PRIu64 " odds/segment; %" PRIu64 " segments\n",
            start,end,threads,seg,j.segments);
    pthread_t ids[MAX_THREADS];
    j.active=threads;
    unsigned created=0;
    for (;created<threads;++created) if (pthread_create(&ids[created],NULL,worker,&j)) {
        pthread_mutex_lock(&j.lock);
        j.active-=threads-created; j.failed=true;
        pthread_cond_broadcast(&j.changed);
        pthread_mutex_unlock(&j.lock);
        break;
    }
    pthread_mutex_lock(&j.lock);
    while (j.active) {
        struct timespec until;
        if (clock_gettime(CLOCK_REALTIME,&until)) die("clock_gettime failed");
        ++until.tv_sec;
        pthread_cond_timedwait(&j.changed,&j.lock,&until);
        double now=seconds();
        if (received_signal()) pthread_cond_broadcast(&j.changed);
        if (now-last_report>=5) {
            uint64_t next=j.first+2*j.published*seg;
            fprintf(stderr,"progress: %" PRIu64 "/%" PRIu64 " segments; "
                    "contiguous=%" PRIu64 "; primes=%" PRIu64 "; %.1fs\n",
                    j.completed,j.segments,next>end ? end : next-1,
                    j.primes,now-begin);
            last_report=now;
        }
    }
    pthread_mutex_unlock(&j.lock);
    for (unsigned i=0;i<created;++i) if (pthread_join(ids[i],NULL)) j.failed=true;
    if (fflush(stdout)) j.failed=true;
    double elapsed=seconds()-begin;
    bool complete=j.published==j.segments && !j.failed;
    fprintf(stderr,"%s: primes=%" PRIu64 "; elapsed=%.3fs; %.0f primes/s; "
            "segments=%" PRIu64 "/%" PRIu64 "\n",
            complete ? "COMPLETE" : j.failed ? "FAILED" : "INTERRUPTED",
            j.primes,elapsed,elapsed>0 ? (double)j.primes/elapsed : 0,
            j.published,j.segments);
    if (complete) {
        fprintf(stderr,"completed inclusive interval: [%" PRIu64 ", %" PRIu64 "]\n",
                start,end);
    } else if (!j.failed) {
        uint64_t next=j.first+2*j.published*seg;
        if (next>start) fprintf(stderr,"completed inclusive interval: [%" PRIu64
                                ", %" PRIu64 "]\n",start,next-1);
        fprintf(stderr,"resume: ./306785_01 %" PRIu64 " %" PRIu64 " %" PRIu64
                " %" PRIu64 " > NEW_OUTPUT_FILE\n",next,end,nt,seg);
    } else {
        fputs("allocation, thread, or output failure; completion is NOT certified\n",stderr);
    }
    for (unsigned i=0;i<j.window;++i) free(j.slots[i].p);
    free(j.slots); free(j.base.p);
    pthread_cond_destroy(&j.changed); pthread_mutex_destroy(&j.lock);
    return j.failed ? EXIT_FAILURE : complete ? 0 : received_signal()==SIGTERM ? 143 : 130;
}
