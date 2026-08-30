#include <gmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HASH_SIZE (1u << 22) /* power of two */

typedef struct {
    uint64_t mask;
    int idx;
    mpz_t val;
    uint32_t gen;
    char initialized;
} Cache;

static Cache *cache;
static uint32_t current_gen = 1;
static int v[64];
static int adj[64][64];
static int adj_len[64];
static int row_order[64];

static int gcd_int(int a, int b) {
    while (b) {
        int t = a % b;
        a = b;
        b = t;
    }
    return (a < 0) ? -a : a;
}

static int is_prime_int(int n) {
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (int i = 3; (int64_t)i * (int64_t)i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

static inline uint64_t get_hash(uint64_t mask, int idx) {
    uint64_t h = mask ^ ((uint64_t)(unsigned)idx * 0x9e3779b97f4a7c15ULL);
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdULL;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ULL;
    h ^= h >> 33;
    return h & (HASH_SIZE - 1);
}

static void next_generation(void) {
    current_gen++;
    if (current_gen == 0) {
        for (uint64_t i = 0; i < HASH_SIZE; i++) cache[i].gen = 0;
        current_gen = 1;
    }
}

static int cache_get(uint64_t mask, int idx, mpz_t out) {
    uint64_t h = get_hash(mask, idx);
    for (uint64_t step = 0; step < HASH_SIZE; step++) {
        Cache *slot = &cache[(h + step) & (HASH_SIZE - 1)];
        if (slot->gen != current_gen) return 0;
        if (slot->mask == mask && slot->idx == idx) {
            mpz_set(out, slot->val);
            return 1;
        }
    }
    return 0;
}

static void cache_put(uint64_t mask, int idx, const mpz_t val) {
    uint64_t h = get_hash(mask, idx);
    for (uint64_t step = 0; step < HASH_SIZE; step++) {
        Cache *slot = &cache[(h + step) & (HASH_SIZE - 1)];
        if (slot->gen != current_gen || (slot->mask == mask && slot->idx == idx)) {
            if (!slot->initialized) {
                mpz_init(slot->val);
                slot->initialized = 1;
            }
            slot->mask = mask;
            slot->idx = idx;
            slot->gen = current_gen;
            mpz_set(slot->val, val);
            return;
        }
    }
    /* table full for this generation: skip memoization */
}

static void permanent_recursive(int idx, uint64_t mask, int m, mpz_t res) {
    if (idx == m) {
        mpz_set_ui(res, 1);
        return;
    }

    if (cache_get(mask, idx, res)) return;

    mpz_set_ui(res, 0);
    mpz_t temp;
    mpz_init(temp);

    int r = row_order[idx];
    for (int i = 0; i < adj_len[r]; i++) {
        int c = adj[r][i];
        if (!(mask & (1ULL << c))) {
            permanent_recursive(idx + 1, mask | (1ULL << c), m, temp);
            mpz_add(res, res, temp);
        }
    }

    cache_put(mask, idx, res);
    mpz_clear(temp);
}

static void solve(int n, FILE *out) {
    int m = 0;
    for (int x = 2; x <= n; x++) {
        if (!is_prime_int(x) || (2 * x <= n)) v[m++] = x;
    }

    if (m == 0) {
        fprintf(out, "%d 1\n", n);
        return;
    }
    if (m > 63) {
        fprintf(stderr, "m=%d exceeds uint64_t mask limit at n=%d\n", m, n);
        exit(1);
    }

    int deg[64] = {0};
    for (int i = 0; i < m; i++) {
        adj_len[i] = 0;
        row_order[i] = i;
        for (int j = 0; j < m; j++) {
            if (gcd_int(v[i], v[j]) > 1) {
                adj[i][adj_len[i]++] = j;
                deg[i]++;
            }
        }
    }

    for (int i = 0; i < m - 1; i++) {
        for (int j = i + 1; j < m; j++) {
            if (deg[row_order[i]] > deg[row_order[j]]) {
                int t = row_order[i];
                row_order[i] = row_order[j];
                row_order[j] = t;
            }
        }
    }

    next_generation();

    mpz_t res;
    mpz_init(res);
    permanent_recursive(0, 0ULL, m, res);
    char *s = mpz_get_str(NULL, 10, res);
    if (!s) {
        fprintf(stderr, "mpz_get_str failed\n");
        mpz_clear(res);
        exit(1);
    }
    fprintf(out, "%d %s\n", n, s);
    free(s);
    mpz_clear(res);
}

int main(void) {
    cache = (Cache *)calloc(HASH_SIZE, sizeof(Cache));
    if (!cache) {
        fprintf(stderr, "cache allocation failed\n");
        return 1;
    }

    FILE *out = fopen("b354830.txt", "w");
    if (!out) {
        fprintf(stderr, "failed to open output file: b354830.txt\n");
        free(cache);
        return 1;
    }

    fprintf(out, "0 1\n");
    fflush(out);
    for (int n = 1; n <= 27; n++) {
        solve(n, out);
        fflush(out);
    }

    for (uint64_t i = 0; i < HASH_SIZE; i++) {
        if (cache[i].initialized) mpz_clear(cache[i].val);
    }
    fclose(out);
    free(cache);
    return 0;
}
