/*
 * 398322_01.c -- Exact memory-intensive enumeration of OEIS A398322,
 *                together with the corresponding A001336 values.
 *
 * Output columns:
 *
 *     n Z_n P_n
 *
 * Z_n is the number of n-step self-avoiding walks on the face-centered
 * cubic (f.c.c.) lattice (OEIS A001336). P_n is the sum of their squared
 * end-to-end distances in the integer-coordinate realization whose twelve
 * steps are the permutations of (+/-1,+/-1,0) (OEIS A398322).
 *
 * METHOD
 * ------
 * This is an independent meet-in-the-middle implementation based on the
 * MIT-licensed 001337_08.c by manman4. That program was independently
 * derived from the published mathematical "length-doubling" idea and
 * contains no SAWdoubler source code. This adaptation likewise uses no
 * SAWdoubler source code.
 *
 * Split an n-step walk at a join point X:
 *
 *     O --N steps--> X --M steps--> Y,       N + M = n.
 *
 * Bag A contains every N-step self-avoiding half-walk O -> X. A read-only
 * template bag B contains every M-step self-avoiding continuation from a
 * relative origin to every endpoint. B is generated and compressed only
 * once, then translated to each X through a checked monotone site-id map.
 * The join X is omitted from both stored vertex sets. The origin O is omitted
 * from A and B rows whose translation touches O are rejected. Therefore a
 * pair forms one full self-avoiding walk exactly when its two stored sets are
 * disjoint.
 *
 * Inclusion-exclusion counts disjoint pairs. In addition to multiplicity,
 * each compressed B row stores its multiplicity, endpoint coordinate sums,
 * and squared-distance sum. Translation by X uses
 *
 *     sum |X+d|^2 = multiplicity*|X|^2 + 2*X dot sum(d) + sum |d|^2.
 *
 * The same recursion therefore returns both the number Z of disjoint pairs
 * and their squared-distance sum P without enumerating endpoint types
 * separately. Since the unrestricted B bag is the larger side, the uniform
 * split rule moves one step from B to the fixed-join A side whenever both
 * halves remain nonempty. This changes only the decomposition point, not the
 * walks being counted.
 *
 * The 48 signed coordinate permutations fixing O preserve the f.c.c.
 * lattice, self-avoidance, and |Y|^2. Join points are reduced to orbits under
 * this group; one representative is evaluated and multiplied by its exact
 * orbit size. OpenMP, when enabled, distributes independent join-point
 * representatives.
 *
 * The branch cut-off changes only whether an inclusion-exclusion subtree is
 * expanded or evaluated directly. It cannot change the mathematical result;
 * the self-test checks this invariance and also compares with a plain DFS.
 *
 * Based on:
 *   001337_08.c, independently written by manman4 under the MIT License.
 *
 * Mathematical reference:
 *   R. D. Schram, G. T. Barkema and R. H. Bisseling,
 *   "Exact enumeration of self-avoiding walks",
 *   J. Stat. Mech. (2011) P06019.
 *
 * Copyright (c) 2026 manman4
 * A398322 adaptation by OpenAI Codex, 2026.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -pedantic 398322_01.c -o 398322_01
 *   gcc-omp 398322_01.c -o 398322_01
 *
 * Usage:
 *   ./398322_01 N
 *   ./398322_01 --upto N
 *   ./398322_01 selftest
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <inttypes.h>

#ifdef _OPENMP
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#endif
#include <omp.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif

typedef __int128 i128;
typedef unsigned __int128 u128;

#define MIN_INDEX 0
#define MAX_INDEX 18
#define MAX_FIXED_HALF_STEPS ((MAX_INDEX + 3) / 2)
#define MAX_BAG_WIDTH (MAX_FIXED_HALF_STEPS - 1)
#define MAX_LATTICE_RADIUS 18
#define DEFAULT_CUTOFF 32U
#define HALF_WALK_LIMIT UINT64_C(28295372292) /* 12 * 11^9 */

_Static_assert(SIZE_MAX >= HALF_WALK_LIMIT,
               "398322_01 requires a 64-bit size_t");

/*
 * Integer bounds for 0 <= n <= 18:
 * - the biased split has fixed-join length at most 10 and unrestricted
 *   continuation length at most 8;
 * - their nonbacktracking bounds are 12*11^9 and 12*11^7;
 * - each multiplicity fits uint64_t and every continuation endpoint
 *   coordinate sum fits int64_t;
 * - a continuation-bag squared-distance sum is below
 *   3*18^2*12*11^7 < 2^38, so uint64_t is sufficient;
 * - every disjoint-pair subtree is below
 *   (12*11^9)*(12*11^7) < 2^63;
 * - Z_n <= 12*11^17 < 2^63;
 * - P_n <= 3*18^2*12*11^17 < 2^73.
 * Signed 128-bit intermediate and output arithmetic therefore has ample
 * range. Allocation products are checked independently.
 */

static void die(const char *what)
{
    fprintf(stderr, "398322_01: %s\n", what);
    exit(EXIT_FAILURE);
}

static size_t checked_product(size_t a, size_t b)
{
    if (a != 0U && b > SIZE_MAX / a) die("allocation size overflow");
    return a * b;
}

static void *xmalloc(size_t n)
{
    void *p = malloc(n ? n : 1);
    if (!p) die("out of memory");
    return p;
}

static void *xmalloc_array(size_t count, size_t size)
{
    return xmalloc(checked_product(count, size));
}

static void *xcalloc_array(size_t count, size_t size)
{
    void *p;
    (void)checked_product(count, size);
    p = calloc(count ? count : 1U, size ? size : 1U);
    if (!p) die("out of memory");
    return p;
}

static void *xrealloc_array(void *old, size_t count, size_t size)
{
    size_t bytes = checked_product(count, size);
    void *p = realloc(old, bytes ? bytes : 1U);
    if (!p) die("out of memory while collecting half-walks");
    return p;
}

/* ------------------------------------------------------------------ */
/* 128-bit decimal output                                             */
/* ------------------------------------------------------------------ */

static void fmt_i128(char *out, i128 v)
{
    char tmp[48];
    int p = 0, neg = 0, i;
    u128 magnitude;

    if (v < 0) {
        neg = 1;
        magnitude = (u128)(-(v + 1)) + 1U;
    } else {
        magnitude = (u128)v;
    }
    if (magnitude == 0U) tmp[p++] = '0';
    while (magnitude > 0U) {
        tmp[p++] = (char)('0' + (int)(magnitude % 10U));
        magnitude /= 10U;
    }
    i = 0;
    if (neg) out[i++] = '-';
    while (p > 0) out[i++] = tmp[--p];
    out[i] = '\0';
}

static void print_i128(i128 v)
{
    char buf[48];
    fmt_i128(buf, v);
    fputs(buf, stdout);
}

/* ------------------------------------------------------------------ */
/* f.c.c. lattice inside a coordinate box                             */
/* ------------------------------------------------------------------ */

/* The 12 nearest neighbours: every permutation of (+-1,+-1,0). */
static const int STEP[12][3] = {
    {  1,  1,  0 }, {  1, -1,  0 }, { -1,  1,  0 }, { -1, -1,  0 },
    {  1,  0,  1 }, {  1,  0, -1 }, { -1,  0,  1 }, { -1,  0, -1 },
    {  0,  1,  1 }, {  0,  1, -1 }, {  0, -1,  1 }, {  0, -1, -1 }
};

typedef struct {
    int      radius;    /* coordinates run over [-radius, radius] */
    int      side;      /* 2*radius + 1                           */
    long     cells;     /* side^3                                 */
    int      nsite;     /* number of lattice sites in the box     */
    int     *cell2id;   /* box cell -> site id, or -1             */
    int16_t *px, *py, *pz;
    int32_t *adj;       /* nsite * 12 -> site id, or -1           */
} Lattice;

static long cell_of(const Lattice *g, int x, int y, int z)
{
    return ((long)(x + g->radius) * g->side + (y + g->radius)) * g->side
           + (z + g->radius);
}

static int site_of(const Lattice *g, int x, int y, int z)
{
    if (x < -g->radius || x > g->radius) return -1;
    if (y < -g->radius || y > g->radius) return -1;
    if (z < -g->radius || z > g->radius) return -1;
    return g->cell2id[cell_of(g, x, y, z)];
}

/* A point belongs to the f.c.c. lattice iff its coordinate sum is even. */
static int on_lattice(int x, int y, int z)
{
    int64_t s = (int64_t)x + (int64_t)y + (int64_t)z;
    return ((s % 2) + 2) % 2 == 0;
}

static void lattice_build(Lattice *g, int radius)
{
    long c;
    int x, y, z, n, d;

    if (radius < 0 || radius > MAX_LATTICE_RADIUS)
        die("internal lattice radius out of range");

    g->radius = radius;
    g->side   = 2 * radius + 1;
    g->cells  = (long)g->side * g->side * g->side;

    g->cell2id = xmalloc_array((size_t)g->cells, sizeof(*g->cell2id));
    for (c = 0; c < g->cells; c++) g->cell2id[c] = -1;

    n = 0;
    for (x = -radius; x <= radius; x++)
        for (y = -radius; y <= radius; y++)
            for (z = -radius; z <= radius; z++)
                if (on_lattice(x, y, z)) n++;

    if (n <= 0) die("internal lattice contains no sites");
    if (n > 65535) die("box too large: site ids must fit in uint16_t");

    g->nsite = n;
    g->px = xmalloc_array((size_t)n, sizeof(*g->px));
    g->py = xmalloc_array((size_t)n, sizeof(*g->py));
    g->pz = xmalloc_array((size_t)n, sizeof(*g->pz));
    memset(g->px, 0, checked_product((size_t)n, sizeof(*g->px)));
    memset(g->py, 0, checked_product((size_t)n, sizeof(*g->py)));
    memset(g->pz, 0, checked_product((size_t)n, sizeof(*g->pz)));

    n = 0;
    for (x = -radius; x <= radius; x++)
        for (y = -radius; y <= radius; y++)
            for (z = -radius; z <= radius; z++)
                if (on_lattice(x, y, z)) {
                    g->cell2id[cell_of(g, x, y, z)] = n;
                    g->px[n] = (int16_t)x;
                    g->py[n] = (int16_t)y;
                    g->pz[n] = (int16_t)z;
                    n++;
                }
    if (n != g->nsite)
        die("internal lattice enumeration count mismatch");

    g->adj = xmalloc_array(
        checked_product((size_t)g->nsite, 12U), sizeof(*g->adj));
    memset(g->adj, 0,
           checked_product(checked_product((size_t)g->nsite, 12U),
                           sizeof(*g->adj)));
    for (n = 0; n < g->nsite; n++)
        for (d = 0; d < 12; d++)
            g->adj[(long)n * 12 + d] =
                site_of(g, g->px[n] + STEP[d][0],
                           g->py[n] + STEP[d][1],
                           g->pz[n] + STEP[d][2]);
}

static void lattice_free(Lattice *g)
{
    free(g->cell2id); free(g->px); free(g->py); free(g->pz); free(g->adj);
}

/* Breadth-first graph distance; unreachable sites get INT_MAX/2. */
static void bfs(const Lattice *g, int src, int *dist, int *queue)
{
    int head = 0, tail = 0, i, d;

    for (i = 0; i < g->nsite; i++) dist[i] = INT_MAX / 2;
    dist[src] = 0;
    queue[tail++] = src;
    while (head < tail) {
        int u = queue[head++];
        const int32_t *nb = g->adj + (long)u * 12;
        for (d = 0; d < 12; d++) {
            int v = nb[d];
            if (v >= 0 && dist[v] > dist[u] + 1) {
                dist[v] = dist[u] + 1;
                queue[tail++] = v;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* Bag of half-walks, each stored as its sorted vertex set             */
/* ------------------------------------------------------------------ */

typedef struct {
    uint16_t *row;   /* count * width entries, each row ascending */
    uint64_t *mult;  /* multiplicity, valid after bag_compress()  */
    uint64_t *weight;/* sum of endpoint squared distances         */
    int64_t  *endx;  /* sum of endpoint x coordinates             */
    int64_t  *endy;  /* sum of endpoint y coordinates             */
    int64_t  *endz;  /* sum of endpoint z coordinates             */
    size_t    count;
    size_t    cap;
    int       width;
    int       track_endpoint;
} Bag;

static void bag_init(Bag *b, int width, int track_endpoint)
{
    if (width < 0 || width > MAX_BAG_WIDTH)
        die("internal half-walk width out of range");
    b->row = NULL; b->mult = NULL; b->weight = NULL;
    b->endx = NULL; b->endy = NULL; b->endz = NULL;
    b->count = 0; b->cap = 0; b->width = width;
    b->track_endpoint = track_endpoint;
}

static void bag_free(Bag *b)
{
    free(b->row); free(b->mult); free(b->weight);
    free(b->endx); free(b->endy); free(b->endz);
}

static void bag_add(Bag *b, const int *path, uint64_t weight,
                    int64_t endx, int64_t endy, int64_t endz)
{
    uint16_t *r;
    int i, j;

    if (!b->track_endpoint &&
        (weight != 0U || endx != 0 || endy != 0 || endz != 0))
        die("internal endpoint data in an unweighted bag");

    if ((uint64_t)b->count >= HALF_WALK_LIMIT)
        die("half-walk count exceeded its proved bound");

    if (b->width == 0) {
        if (weight != 0U || endx != 0 || endy != 0 || endz != 0)
            die("internal nonzero endpoint data for a width-zero row");
        b->count++;
        return;
    }

    if (b->count == b->cap) {
        size_t cap;
        uint16_t *grown;
        uint64_t *grown_weight;
        int64_t *grown_endx, *grown_endy, *grown_endz;
        if (b->cap == 0U) {
            cap = 4096U;
        } else {
            if (b->cap > SIZE_MAX / 2U) die("half-walk capacity overflow");
            cap = b->cap * 2U;
        }
        if ((uint64_t)cap > HALF_WALK_LIMIT)
            cap = (size_t)HALF_WALK_LIMIT;
        grown = xrealloc_array(
            b->row,
            checked_product(cap, (size_t)b->width),
            sizeof(*b->row));
        grown_weight = b->weight;
        grown_endx = b->endx;
        grown_endy = b->endy;
        grown_endz = b->endz;
        if (b->track_endpoint) {
            grown_weight = xrealloc_array(
                b->weight, cap, sizeof(*b->weight));
            grown_endx = xrealloc_array(b->endx, cap, sizeof(*b->endx));
            grown_endy = xrealloc_array(b->endy, cap, sizeof(*b->endy));
            grown_endz = xrealloc_array(b->endz, cap, sizeof(*b->endz));
        }
        b->row = grown;
        b->weight = grown_weight;
        b->endx = grown_endx;
        b->endy = grown_endy;
        b->endz = grown_endz;
        b->cap = cap;
    }
    r = b->row + b->count * (size_t)b->width;
    for (i = 0; i < b->width; i++) r[i] = (uint16_t)path[i];
    for (i = 1; i < b->width; i++) {           /* insertion sort, width <= 9 */
        uint16_t v = r[i];
        j = i - 1;
        while (j >= 0 && r[j] > v) { r[j + 1] = r[j]; j--; }
        r[j + 1] = v;
    }
    if (b->track_endpoint) {
        b->weight[b->count] = weight;
        b->endx[b->count] = endx;
        b->endy[b->count] = endy;
        b->endz[b->count] = endz;
    }
    b->count++;
}

static int64_t checked_add_i64(int64_t a, int64_t b)
{
    if ((b > 0 && a > INT64_MAX - b) ||
        (b < 0 && a < INT64_MIN - b))
        die("half-walk endpoint-coordinate sum overflow");
    return a + b;
}

/* Sort rows lexicographically (LSD radix over the columns) and merge
   duplicates into multiplicities.  Distinct walks with the same vertex set
   are interchangeable for the disjointness count. */
static void bag_compress(Bag *b, int nsite)
{
    size_t n = b->count, i, m;
    int w = b->width, col, s;
    size_t *ord, *scratch;
    size_t *bucket;
    uint16_t *out;
    uint64_t *mul, *weight;
    int64_t *endx, *endy, *endz;

    if (n == 0) {
        b->mult = NULL; b->weight = NULL;
        b->endx = NULL; b->endy = NULL; b->endz = NULL;
        return;
    }
    if (w == 0) {                                /* one empty set, n copies */
        b->mult = xmalloc_array(1U, sizeof(*b->mult));
        b->mult[0] = (uint64_t)n;
        if (b->track_endpoint) {
            b->weight = xmalloc_array(1U, sizeof(*b->weight));
            b->endx = xmalloc_array(1U, sizeof(*b->endx));
            b->endy = xmalloc_array(1U, sizeof(*b->endy));
            b->endz = xmalloc_array(1U, sizeof(*b->endz));
            b->weight[0] = 0U;
            b->endx[0] = 0; b->endy[0] = 0; b->endz[0] = 0;
        }
        b->count = 1U;
        return;
    }

    ord     = xmalloc_array(n, sizeof(*ord));
    scratch = xmalloc_array(n, sizeof(*scratch));
    bucket  = xcalloc_array((size_t)nsite + 1U, sizeof(*bucket));

    for (i = 0; i < n; i++) ord[i] = i;
    for (col = w - 1; col >= 0; col--) {
        if (col != w - 1)
            memset(bucket, 0,
                   checked_product((size_t)nsite + 1U, sizeof(*bucket)));
        for (i = 0; i < n; i++)
            bucket[b->row[
                ord[i] * (size_t)w + (size_t)col] + 1U]++;
        for (s = 0; s < nsite; s++) bucket[s + 1] += bucket[s];
        for (i = 0; i < n; i++)
            scratch[bucket[b->row[
                ord[i] * (size_t)w + (size_t)col]]++] = ord[i];
        memcpy(ord, scratch, checked_product(n, sizeof(*ord)));
    }

    out = xmalloc_array(
        checked_product(n, (size_t)w), sizeof(*out));
    mul = xmalloc_array(n, sizeof(*mul));
    weight = NULL; endx = NULL; endy = NULL; endz = NULL;
    if (b->track_endpoint) {
        weight = xmalloc_array(n, sizeof(*weight));
        endx = xmalloc_array(n, sizeof(*endx));
        endy = xmalloc_array(n, sizeof(*endy));
        endz = xmalloc_array(n, sizeof(*endz));
    }
    m = 0;
    for (i = 0; i < n; i++) {
        const uint16_t *r = b->row + ord[i] * (size_t)w;
        if (m > 0U &&
            memcmp(out + (m - 1U) * (size_t)w, r,
                   checked_product((size_t)w, sizeof(*out))) == 0) {
            if (mul[m - 1U] == UINT64_MAX)
                die("half-walk multiplicity overflow");
            mul[m - 1U]++;
            if (b->track_endpoint) {
                if (UINT64_MAX - weight[m - 1U] < b->weight[ord[i]])
                    die("half-walk endpoint-weight overflow");
                weight[m - 1U] += b->weight[ord[i]];
                endx[m - 1U] =
                    checked_add_i64(endx[m - 1U], b->endx[ord[i]]);
                endy[m - 1U] =
                    checked_add_i64(endy[m - 1U], b->endy[ord[i]]);
                endz[m - 1U] =
                    checked_add_i64(endz[m - 1U], b->endz[ord[i]]);
            }
        } else {
            memcpy(out + m * (size_t)w, r,
                   checked_product((size_t)w, sizeof(*out)));
            mul[m] = 1U;
            if (b->track_endpoint) {
                weight[m] = b->weight[ord[i]];
                endx[m] = b->endx[ord[i]];
                endy[m] = b->endy[ord[i]];
                endz[m] = b->endz[ord[i]];
            }
            m++;
        }
    }

    free(ord); free(scratch); free(bucket); free(b->row);
    free(b->weight); free(b->endx); free(b->endy); free(b->endz);
    b->row = out; b->mult = mul; b->weight = weight;
    b->endx = endx; b->endy = endy; b->endz = endz; b->count = m;
}

/* ------------------------------------------------------------------ */
/* Half-walk generation                                                */
/* ------------------------------------------------------------------ */

typedef struct {
    const Lattice *g;
    const int     *dist;   /* graph distance to the join point */
    unsigned char *seen;
    int            steps;  /* walk length                      */
    int           *path;   /* path[0..steps]                   */
    int            banned; /* the opposite endpoint; see below */
    Bag           *bag;
} Walker;

static void walk_rec(Walker *w, int depth)
{
    int here = w->path[depth];
    int left = w->steps - depth;
    const int32_t *nb;
    int d;

    /* Cannot possibly reach the join point in the steps that remain. */
    if (w->dist[here] > left) return;

    if (depth == w->steps) {
        /* dist == 0 here, so this vertex is the join point.  Neither it nor
           path[0] is stored: path[0] is the walk's own start point, which by
           construction lies on every row of this bag and on no row of the
           other one (walks through the opposite endpoint were rejected
           below), so it can never be a shared site.  Dropping both keeps the
           row width at steps-1. */
        bag_add(w->bag, w->path + 1, 0U, 0, 0, 0);
        return;
    }

    w->seen[here] = 1;
    nb = w->g->adj + (long)here * 12;
    for (d = 0; d < 12; d++) {
        int next = nb[d];
        /* A walk that touches the opposite endpoint would meet every row of
           the other bag there, so no pair containing it can be disjoint. */
        if (next >= 0 && next != w->banned && !w->seen[next]) {
            w->path[depth + 1] = next;
            walk_rec(w, depth + 1);
        }
    }
    w->seen[here] = 0;
}

typedef struct {
    const Lattice *g;
    unsigned char *seen;
    int            steps;
    int           *path;
    int            banned;
    Bag           *bag;
} ContinuationWalker;

/* Generate every self-avoiding continuation from the join point. The stored
   row contains path[1..steps], including the final endpoint. */
static void continuation_rec(ContinuationWalker *w, int depth)
{
    int here = w->path[depth];
    const int32_t *nb;
    int d;

    if (depth == w->steps) {
        int64_t x = w->g->px[here];
        int64_t y = w->g->py[here];
        int64_t z = w->g->pz[here];
        uint64_t r2 = (uint64_t)(x * x + y * y + z * z);
        bag_add(w->bag, w->path + 1, r2, x, y, z);
        return;
    }

    w->seen[here] = 1;
    nb = w->g->adj + (long)here * 12;
    for (d = 0; d < 12; d++) {
        int next = nb[d];
        if (next >= 0 && next != w->banned && !w->seen[next]) {
            w->path[depth + 1] = next;
            continuation_rec(w, depth + 1);
        }
    }
    w->seen[here] = 0;
}

/* ------------------------------------------------------------------ */
/* Disjoint-pair counting                                              */
/* ------------------------------------------------------------------ */

typedef struct {
    const Bag *A, *B;
    size_t    *aidx, *bidx;    /* index lists, permuted in place */
    const int *b_site_map;      /* relative B site -> translated global site */
    const uint64_t *b_weight;   /* translated endpoint weight per B row */
    uint64_t  *seenA, *seenB;  /* epoch stamps over the sites    */
    uint16_t  *cand;           /* per-depth candidate buffers    */
    uint64_t   epoch;
    int        nsite;
    size_t     cutoff;         /* switch to direct counting here */
} Counter;

typedef struct {
    i128 z;
    i128 p;
} PairSum;

static const uint16_t EMPTY_ROW[1] = { 0U };

static const uint16_t *bag_row_at(const Bag *b, size_t index)
{
    if (b->width == 0) return EMPTY_ROW;
    return b->row + index * (size_t)b->width;
}

/* Do a and b share an element strictly greater than "last"? */
static int shares_above(const uint16_t *a, int ka,
                        const uint16_t *b, int kb, int last,
                        const int *b_site_map)
{
    int i = 0, j = 0;

    while (i < ka && a[i] <= last) i++;
    while (j < kb && b_site_map[b[j]] <= last) j++;
    while (i < ka && j < kb) {
        int mapped_b = b_site_map[b[j]];
        if (a[i] == mapped_b) return 1;
        if (a[i] < mapped_b) i++; else j++;
    }
    return 0;
}

/* Move every row containing "s" to the front of [lo,hi); return the split.
   Only the order changes, so the range may be re-partitioned afterwards. */
static size_t partition_on(size_t *idx, size_t lo, size_t hi,
                           const uint16_t *rows, int width, int s,
                           const int *site_map)
{
    size_t i, w = lo;

    if (width == 0) return lo;

    for (i = lo; i < hi; i++) {
        const uint16_t *r = rows + idx[i] * (size_t)width;
        int j, hit = 0;
        for (j = 0; j < width; j++)
            if ((site_map == NULL ? (int)r[j] : site_map[r[j]]) == s) {
                hit = 1;
                break;
            }
        if (hit) {
            size_t t = idx[w]; idx[w] = idx[i]; idx[i] = t;
            w++;
        }
    }
    return w;
}

/*
 * Value of the subtree hanging below a prefix set whose largest element is
 * "last", with the sign (-1)^|prefix| factored out:
 *
 *     sum over T subset of {elements > last} of
 *         (-1)^|T| * f(prefix+T) * g(prefix+T)
 *
 *   = #{ (a,b) in current lists : a n b has no element > last }.
 */
static PairSum subtree(Counter *c, size_t alo, size_t ahi,
                       size_t blo, size_t bhi,
                       int last, int depth)
{
    size_t na = ahi - alo, nb = bhi - blo, i, j;
    int ka = c->A->width, kb = c->B->width;
    i128 sa = 0, sb = 0, sp = 0;
    PairSum total = { 0, 0 };
    uint64_t stamp;
    uint16_t *cand;
    int ncand = 0, k;

    if (na == 0U || nb == 0U) return total;

    if (na <= c->cutoff / nb) {                    /* direct evaluation */
        for (i = alo; i < ahi; i++) {
            const uint16_t *a = bag_row_at(c->A, c->aidx[i]);
            uint64_t ma = c->A->mult[c->aidx[i]];
            for (j = blo; j < bhi; j++) {
                const uint16_t *b = bag_row_at(c->B, c->bidx[j]);
                if (!shares_above(a, ka, b, kb, last, c->b_site_map)) {
                    total.z += (i128)ma * c->B->mult[c->bidx[j]];
                    total.p += (i128)ma * c->b_weight[c->bidx[j]];
                }
            }
        }
        return total;
    }

    for (i = alo; i < ahi; i++) sa += c->A->mult[c->aidx[i]];
    for (j = blo; j < bhi; j++) {
        sb += c->B->mult[c->bidx[j]];
        sp += c->b_weight[c->bidx[j]];
    }
    total.z = sa * sb;                              /* the T = {} term */
    total.p = sa * sp;

    /* Candidates: sites above "last" present on both sides. */
    if (c->epoch == UINT64_MAX) {
        memset(c->seenA, 0,
               checked_product((size_t)c->nsite, sizeof(*c->seenA)));
        memset(c->seenB, 0,
               checked_product((size_t)c->nsite, sizeof(*c->seenB)));
        c->epoch = 0U;
    }
    stamp = ++c->epoch;
    cand  = c->cand + (size_t)depth * (size_t)c->nsite;
    for (i = alo; i < ahi; i++) {
        const uint16_t *a = bag_row_at(c->A, c->aidx[i]);
        for (k = 0; k < ka; k++)
            if (a[k] > last) c->seenA[a[k]] = stamp;
    }
    for (j = blo; j < bhi; j++) {
        const uint16_t *b = bag_row_at(c->B, c->bidx[j]);
        for (k = 0; k < kb; k++) {
            int e = c->b_site_map[b[k]];
            if (e > last && c->seenA[e] == stamp && c->seenB[e] != stamp) {
                c->seenB[e] = stamp;
                cand[ncand++] = (uint16_t)e;
            }
        }
    }
    for (k = 1; k < ncand; k++) {                   /* small, ascending */
        uint16_t v = cand[k];
        int m = k - 1;
        while (m >= 0 && cand[m] > v) { cand[m + 1] = cand[m]; m--; }
        cand[m + 1] = v;
    }

    for (k = 0; k < ncand; k++) {
        int s = cand[k];
        size_t amid =
            partition_on(c->aidx, alo, ahi, c->A->row, ka, s, NULL);
        size_t bmid = partition_on(c->bidx, blo, bhi, c->B->row, kb, s,
                                   c->b_site_map);
        PairSum child =
            subtree(c, alo, amid, blo, bmid, s, depth + 1);
        total.z -= child.z;
        total.p -= child.p;
    }
    if (total.z < 0 || total.p < 0)
        die("internal inclusion-exclusion invariant failed");
    return total;
}

/* ------------------------------------------------------------------ */
/* Full point group fixing the origin                                 */
/* ------------------------------------------------------------------ */

static const int PERM[6][3] = {
    {0,1,2}, {0,2,1}, {1,0,2}, {1,2,0}, {2,0,1}, {2,1,0}
};

/* g = 8*p + s enumerates all 48 signed coordinate permutations, each of
   which maps the f.c.c. lattice onto itself and fixes the origin. */
static void sym_apply(int gsym, const int *v, int *out)
{
    const int *p = PERM[gsym >> 3];
    int s = gsym & 7, i;

    for (i = 0; i < 3; i++) {
        int val = v[p[i]];
        out[i] = (s >> i) & 1 ? -val : val;
    }
}

/* Is site "id" the canonical representative of its orbit?  If so, report
   how many distinct points the orbit contains. */
static int orbit_rep(const Lattice *g, int id, int *orbit_size)
{
    int v[3], img[3], i, j, best = id, distinct = 0;
    int seen[48];

    v[0] = g->px[id]; v[1] = g->py[id]; v[2] = g->pz[id];
    for (i = 0; i < 48; i++) {
        int other;
        sym_apply(i, v, img);
        other = site_of(g, img[0], img[1], img[2]);
        if (other < 0) return 0;              /* orbit leaves the box */
        seen[i] = other;
        if (other < best) best = other;
    }
    for (i = 0; i < 48; i++) {
        int fresh = 1;
        for (j = 0; j < i; j++) if (seen[j] == seen[i]) { fresh = 0; break; }
        if (fresh) distinct++;
    }
    *orbit_size = distinct;
    return best == id;
}

/* ------------------------------------------------------------------ */
/* Driver                                                              */
/* ------------------------------------------------------------------ */

/* Build all M-step continuations once in coordinates relative to their
   start. Translation to a particular join point preserves row equality and
   lexicographic row order, so the compressed bag is immutable and shareable
   by every OpenMP worker. */
static size_t build_continuation_template(int steps, Lattice *g, Bag *bag)
{
    ContinuationWalker w;
    unsigned char *seen;
    int *path;
    int origin;
    size_t raw_count;

    lattice_build(g, steps);
    origin = site_of(g, 0, 0, 0);
    if (origin < 0) die("internal continuation origin is missing");

    seen = xcalloc_array((size_t)g->nsite, sizeof(*seen));
    path = xmalloc_array((size_t)steps + 1U, sizeof(*path));
    bag_init(bag, steps, 1);

    w.g = g; w.seen = seen; w.steps = steps;
    w.path = path; w.banned = -1; w.bag = bag;
    path[0] = origin;
    continuation_rec(&w, 0);

    raw_count = bag->count;
    if ((uint64_t)raw_count > HALF_WALK_LIMIT)
        die("continuation template exceeded its proved bound");
    bag_compress(bag, g->nsite);

    free(path);
    free(seen);
    return raw_count;
}

typedef struct {
    Lattice   *g;
    const Lattice *continuation_lattice;
    const Bag *continuations;
    int        origin;
    int        nsteps_a, nsteps_b;
    size_t     cutoff;
    uint64_t   max_walks;
} Problem;

/* Count all full walks that pass through join point "join", and sum the
   squared norms of their unrestricted final endpoints. */
static PairSum solve_join(const Problem *pr, int join)
{
    const Lattice *g = pr->g;
    const Lattice *continuation_lattice = pr->continuation_lattice;
    const Bag *B = pr->continuations;
    Bag A;
    Walker w;
    Counter c;
    int *dist, *queue, *path;
    int *b_site_map;
    unsigned char *seen;
    size_t *aidx, *bidx;
    uint64_t *b_weight;
    uint64_t *seenA, *seenB;
    uint16_t *cand;
    int banned_relative, maxdepth;
    int join_x = g->px[join];
    int join_y = g->py[join];
    int join_z = g->pz[join];
    int64_t join_r2 =
        (int64_t)join_x * join_x +
        (int64_t)join_y * join_y +
        (int64_t)join_z * join_z;
    size_t bcount = 0, i;
    PairSum answer = { 0, 0 };

    dist  = xmalloc_array((size_t)g->nsite, sizeof(*dist));
    queue = xmalloc_array((size_t)g->nsite, sizeof(*queue));
    seen  = xmalloc_array((size_t)g->nsite, sizeof(*seen));
    path  = xmalloc_array(
        (size_t)pr->nsteps_a + 1U, sizeof(*path));
    memset(seen, 0, (size_t)g->nsite);

    bfs(g, join, dist, queue);

    bag_init(&A, pr->nsteps_a - 1, 0);

    w.g = g; w.dist = dist; w.seen = seen; w.path = path;

    w.steps = pr->nsteps_a; w.bag = &A; w.banned = -1;
    path[0] = pr->origin;
    walk_rec(&w, 0);

    if (A.count == 0 || B->count == 0) {
        bag_free(&A);
        free(dist); free(queue); free(seen); free(path);
        return answer;
    }
    if (pr->max_walks > 0U && (uint64_t)A.count > pr->max_walks)
        die("half-walk bag exceeded the configured limit");

    bag_compress(&A, g->nsite);

    maxdepth = (A.width < B->width ? A.width : B->width) + 2;
    aidx  = xmalloc_array(A.count, sizeof(*aidx));
    bidx  = xmalloc_array(B->count, sizeof(*bidx));
    b_weight = xmalloc_array(B->count, sizeof(*b_weight));
    b_site_map = xmalloc_array(
        (size_t)continuation_lattice->nsite, sizeof(*b_site_map));
    seenA = xmalloc_array((size_t)g->nsite, sizeof(*seenA));
    seenB = xmalloc_array((size_t)g->nsite, sizeof(*seenB));
    cand  = xmalloc_array(
        checked_product((size_t)g->nsite, (size_t)maxdepth),
        sizeof(*cand));
    memset(seenA, 0,
           checked_product((size_t)g->nsite, sizeof(*seenA)));
    memset(seenB, 0,
           checked_product((size_t)g->nsite, sizeof(*seenB)));

    /*
     * Translation by the join point preserves lexicographic coordinate
     * order. Verify that the resulting site-id map is strictly increasing;
     * shares_above() relies on every translated B row remaining sorted.
     */
    for (i = 0; i < (size_t)continuation_lattice->nsite; i++) {
        int mapped = site_of(
            g, join_x + continuation_lattice->px[i],
               join_y + continuation_lattice->py[i],
               join_z + continuation_lattice->pz[i]);
        if (mapped < 0)
            die("translated continuation left the proved lattice box");
        if (i > 0U && mapped <= b_site_map[i - 1U])
            die("continuation translation did not preserve site order");
        b_site_map[i] = mapped;
    }

    banned_relative = site_of(
        continuation_lattice, -join_x, -join_y, -join_z);
    for (i = 0; i < B->count; i++) {
        const uint16_t *row = bag_row_at(B, i);
        int banned = 0;
        int k;
        i128 translated_weight;

        if (banned_relative >= 0) {
            for (k = 0; k < B->width; k++) {
                if (row[k] == (uint16_t)banned_relative) {
                    banned = 1;
                    break;
                }
            }
        }
        if (banned) continue;

        translated_weight =
            (i128)B->weight[i] + (i128)B->mult[i] * join_r2 +
            (i128)2 * ((i128)join_x * B->endx[i] +
                       (i128)join_y * B->endy[i] +
                       (i128)join_z * B->endz[i]);
        if (translated_weight < 0 ||
            (u128)translated_weight > (u128)UINT64_MAX)
            die("translated continuation weight is outside uint64_t range");
        b_weight[i] = (uint64_t)translated_weight;
        bidx[bcount++] = i;
    }

    if (bcount == 0U) {
        free(b_site_map); free(b_weight); free(aidx); free(bidx);
        free(seenA); free(seenB); free(cand);
        bag_free(&A);
        free(dist); free(queue); free(seen); free(path);
        return answer;
    }

    for (i = 0; i < A.count; i++) aidx[i] = i;

    c.A = &A; c.B = B;
    c.aidx = aidx; c.bidx = bidx;
    c.b_site_map = b_site_map; c.b_weight = b_weight;
    c.seenA = seenA; c.seenB = seenB; c.cand = cand;
    c.epoch = 0U; c.nsite = g->nsite; c.cutoff = pr->cutoff;

    answer = subtree(&c, 0, A.count, 0, bcount, -1, 0);

    free(b_site_map); free(b_weight);
    free(aidx); free(bidx); free(seenA); free(seenB); free(cand);
    bag_free(&A);
    free(dist); free(queue); free(seen); free(path);
    return answer;
}

/* Count all n-step f.c.c. SAWs and their squared endpoint-distance sum. */
static PairSum count_all_walks(int nsteps, size_t cutoff, int verbose)
{
    Lattice g, continuation_lattice;
    Bag continuations;
    Problem pr;
    int na, nb, i, origin;
    int *dist_o, *queue;
    int *reps, nreps = 0;
    int *repsize;
    PairSum total = { 0, 0 };
    i128 total_z = 0, total_p = 0;
    size_t raw_continuations;

    if (nsteps < MIN_INDEX || nsteps > MAX_INDEX)
        die("internal index out of range");
    if (nsteps == 0) {
        total.z = 1;
        return total;
    }
    if (nsteps == 1) {
        total.z = 12;
        total.p = 24;
        return total;
    }

    /*
     * The unrestricted B bag is much larger than a fixed-join A bag. Bias
     * the split by one step toward A whenever both halves remain nonempty.
     * This uniform rule reduces B by roughly the connective constant without
     * changing the set of full walks or any inclusion-exclusion identity.
     */
    nb = nsteps / 2;
    if (nb > 1) nb--;
    na = nsteps - nb;               /* O -> X; X -> unrestricted Y */

    /* Every coordinate changes by at most one per step. The radius-n box is
       therefore sufficient for every complete n-step walk. */
    lattice_build(&g, nsteps);
    raw_continuations =
        build_continuation_template(nb, &continuation_lattice,
                                    &continuations);

    origin = site_of(&g, 0, 0, 0);

    dist_o = xmalloc_array((size_t)g.nsite, sizeof(*dist_o));
    queue  = xmalloc_array((size_t)g.nsite, sizeof(*queue));
    bfs(&g, origin, dist_o, queue);

    reps    = xmalloc_array((size_t)g.nsite, sizeof(*reps));
    repsize = xmalloc_array((size_t)g.nsite, sizeof(*repsize));
    for (i = 0; i < g.nsite; i++) {
        int osz;
        if (dist_o[i] > na) continue;
        if (!orbit_rep(&g, i, &osz)) continue;
        reps[nreps] = i;
        repsize[nreps] = osz;
        nreps++;
    }

    if (verbose)
        fprintf(stderr,
                "  n=%d split=%d+%d radius=%d sites=%d join_orbits=%d "
                "continuations=%zu compressed=%zu\n",
                nsteps, na, nb, nsteps, g.nsite, nreps,
                raw_continuations, continuations.count);

    pr.g = &g;
    pr.continuation_lattice = &continuation_lattice;
    pr.continuations = &continuations;
    pr.origin = origin;
    pr.nsteps_a = na; pr.nsteps_b = nb;
    pr.cutoff = cutoff; pr.max_walks = HALF_WALK_LIMIT;

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1) reduction(+:total_z,total_p)
#endif
    for (i = 0; i < nreps; i++) {
        PairSum one = solve_join(&pr, reps[i]);
        total_z += (i128)repsize[i] * one.z;
        total_p += (i128)repsize[i] * one.p;
    }

    total.z = total_z;
    total.p = total_p;

    free(dist_o); free(queue); free(reps); free(repsize);
    bag_free(&continuations);
    lattice_free(&continuation_lattice);
    lattice_free(&g);
    return total;
}

/* ------------------------------------------------------------------ */
/* Reference values and self-test                                      */
/* ------------------------------------------------------------------ */

/* Table II of Schram et al. (2017), kept to n=12 for a quick self-test.
   Z is A001336 and P is A398322. */
static const uint64_t KNOWN_Z[] = {
    1ULL, 12ULL, 132ULL, 1404ULL, 14700ULL, 152532ULL, 1573716ULL,
    16172148ULL, 165697044ULL, 1693773924ULL, 17281929564ULL,
    176064704412ULL, 1791455071068ULL
};

static const uint64_t KNOWN_P[] = {
    0ULL, 24ULL, 576ULL, 9816ULL, 144288ULL, 1951560ULL, 25021536ULL,
    309080808ULL, 3714659040ULL, 43714781448ULL, 505948384608ULL,
    5777220825912ULL, 65234797723584ULL
};

/*
 * Independent reference: plain depth-first enumeration with no splitting,
 * no inclusion-exclusion and no symmetry reduction.  Exponential, but it
 * shares no logic with the fast path, so agreement between the two checks
 * the whole method rather than one component of it.
 */
typedef struct {
    const Lattice *g;
    unsigned char *seen;
    int  steps;
    PairSum total;
} Brute;

static void brute_rec(Brute *b, int here, int depth)
{
    const int32_t *nb;
    int d;

    if (depth == b->steps) {
        int64_t x = b->g->px[here];
        int64_t y = b->g->py[here];
        int64_t z = b->g->pz[here];
        b->total.z++;
        b->total.p += x * x + y * y + z * z;
        return;
    }
    b->seen[here] = 1;
    nb = b->g->adj + (long)here * 12;
    for (d = 0; d < 12; d++) {
        int next = nb[d];
        if (next >= 0 && !b->seen[next]) brute_rec(b, next, depth + 1);
    }
    b->seen[here] = 0;
}

static PairSum brute_count_all(int nsteps)
{
    Lattice g;
    Brute b;
    int origin;

    lattice_build(&g, nsteps);
    origin = site_of(&g, 0, 0, 0);
    b.g = &g;
    b.seen = xmalloc_array((size_t)g.nsite, sizeof(*b.seen));
    memset(b.seen, 0, (size_t)g.nsite);
    b.steps = nsteps;
    b.total.z = 0;
    b.total.p = 0;
    brute_rec(&b, origin, 0);
    free(b.seen);
    lattice_free(&g);
    return b.total;
}

static int selftest(size_t cutoff)
{
    int n, bad = 0;

    printf("[1] against Table II / OEIS A001336 and A398322\n");
    for (n = 0; n <= 10; n++) {                 /* keep the test quick */
        PairSum got = count_all_walks(n, cutoff, 0);
        int ok = got.z == (i128)KNOWN_Z[n] && got.p == (i128)KNOWN_P[n];
        printf("     n=%2d  Z=", n); print_i128(got.z);
        printf("  P="); print_i128(got.p);
        printf("  %s\n", ok ? "ok" : "MISMATCH");
        if (!ok) bad = 1;
    }

    printf("[2] against plain depth-first enumeration\n");
    for (n = 0; n <= 8; n++) {
        PairSum fast = count_all_walks(n, cutoff, 0);
        PairSum slow = brute_count_all(n);
        int ok = fast.z == slow.z && fast.p == slow.p;
        printf("     n=%2d  Z=", n); print_i128(fast.z);
        printf("  P="); print_i128(fast.p);
        printf("  %s\n", ok ? "ok" : "MISMATCH");
        if (!ok) bad = 1;
    }

    printf("[3] cut-off invariance, both results must be unchanged\n");
    {
        size_t trial[4] = { 1U, 64U, 4096U, 1000000U };
        PairSum ref = { 0, 0 };
        int k;
        for (k = 0; k < 4; k++) {
            PairSum got = count_all_walks(9, trial[k], 0);
            if (k == 0) ref = got;
            printf("     cutoff=%-8zu Z=", trial[k]); print_i128(got.z);
            printf("  P="); print_i128(got.p);
            printf("  %s\n",
                   got.z == ref.z && got.p == ref.p ? "ok" : "MISMATCH");
            if (got.z != ref.z || got.p != ref.p) bad = 1;
        }
    }

    printf("%s\n", bad ? "SELFTEST FAILED" : "selftest passed");
    return bad;
}

/* ------------------------------------------------------------------ */

static void usage(const char *prog)
{
    fprintf(stderr,
        "usage: %s [--cutoff K] [--quiet] N\n"
        "       %s [--cutoff K] [--quiet] --upto N\n"
        "       %s [--cutoff K] selftest\n"
        "where %d <= N <= %d\n",
        prog, prog, prog, MIN_INDEX, MAX_INDEX);
}

static long parse_long_range(const char *text, long minimum, long maximum,
                             const char *description)
{
    char *end = NULL;
    long value;

    if (text == NULL || *text == '\0') die(description);
    errno = 0;
    value = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0')
        die(description);
    if (value < minimum || value > maximum) die(description);
    return value;
}

static size_t parse_cutoff(const char *text)
{
    char *end = NULL;
    uintmax_t value;

    if (text == NULL || *text == '\0' || *text == '-')
        die("--cutoff must be a positive integer");
    errno = 0;
    value = strtoumax(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' || value == 0U ||
        value > (uintmax_t)SIZE_MAX)
        die("--cutoff must be a positive integer");
    return (size_t)value;
}

static i128 nonbacktracking_bound(int n)
{
    i128 bound;
    int i;

    if (n == 0) return 1;
    bound = 12;
    for (i = 1; i < n; i++) bound *= 11;
    return bound;
}

static void print_term(int n, size_t cutoff, int verbose)
{
    PairSum value = count_all_walks(n, cutoff, verbose);
    i128 z_bound = nonbacktracking_bound(n);
    i128 p_bound = (i128)3 * n * n * z_bound;

    if (value.z < 0 || value.p < 0 ||
        value.z > z_bound || value.p > p_bound)
        die("result exceeds its proved nonbacktracking bound");
    printf("%d ", n);
    print_i128(value.z);
    putchar(' ');
    print_i128(value.p);
    putchar('\n');
}

int main(int argc, char **argv)
{
    size_t cutoff = DEFAULT_CUTOFF;
    int verbose = 1;
    int i = 1;

    while (i < argc && strncmp(argv[i], "--", 2U) == 0) {
        if (strcmp(argv[i], "--cutoff") == 0) {
            if (i + 1 >= argc) die("--cutoff requires an argument");
            cutoff = parse_cutoff(argv[i + 1]);
            i += 2;
        } else if (strcmp(argv[i], "--quiet") == 0) {
            verbose = 0;
            i++;
        } else if (strcmp(argv[i], "--upto") == 0) {
            break;
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (i < argc && strcmp(argv[i], "--upto") == 0) {
        int n, limit;
        if (i + 2 != argc) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        limit = (int)parse_long_range(
            argv[i + 1], MIN_INDEX, MAX_INDEX,
            "upper index must be an integer from 0 to 18");
        for (n = MIN_INDEX; n <= limit; n++)
            print_term(n, cutoff, verbose);
        return EXIT_SUCCESS;
    }

    if (i < argc && strcmp(argv[i], "selftest") == 0) {
        if (i + 1 != argc) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        return selftest(cutoff) ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    if (i < argc) {
        int n;
        if (i + 1 != argc) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        n = (int)parse_long_range(
            argv[i], MIN_INDEX, MAX_INDEX,
            "index must be an integer from 0 to 18");
        print_term(n, cutoff, verbose);
        return EXIT_SUCCESS;
    }

    usage(argv[0]);
    return EXIT_FAILURE;
}
