/*
 * A306477: number of ways to write n as
 *   C(w+2,2) + C(x+3,4) + C(y+5,6) + C(z+7,8),  w,x,y,z >= 0.
 *
 * Usage:
 *   ./a306477              -> a(1..80), a(23343989), a(896315812331399)
 *   ./a306477 N [N2 ...]   -> a(N) for each argument
 *   N must be a decimal integer in 0..1000000000000000.
 *
 * Build: gcc -std=c11 -O2 -Wall -Wextra -o a306477 a306477.c -lm
 * Requires the GCC/Clang 128-bit integer extension.
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef uint64_t u64;
typedef __uint128_t u128;
static const u64 max_n = UINT64_C(1000000000000000);

/* Reject signs, whitespace, trailing characters and values above max_n. */
static int parse_n(const char *text, u64 *value) {
    u64 n = 0;
    if (*text == '\0') return 0;
    for (; *text != '\0'; text++) {
        if (*text < '0' || *text > '9') return 0;
        const u64 digit = (u64)(*text - '0');
        if (n > (max_n - digit) / 10) return 0;
        n = 10 * n + digit;
    }
    *value = n;
    return 1;
}

/* exact test: is m a triangular number C(w+2,2) with w >= 0, i.e. m >= 1 and 8m+1 a square */
static int is_tri(u64 m) {
    if (m == 0) return 0;
    /* Wide arithmetic also makes this helper exact for all uint64_t inputs. */
    u128 t = (u128)8 * m + 1;
    u64 s = (u64)sqrtl((long double)t);
    while ((u128)s * s > t) s--;
    while ((u128)(s + 1) * (s + 1) <= t) s++;
    return (u128)s * s == t;
}

/* table of C(i+k-1, k) for i = 0,1,... while value < limit (starts 0, 1, k+1, ...) */
static u64 *table(unsigned k, u64 limit, size_t *len) {
    size_t cap = 1024, n = 0;
    u64 *a = malloc(cap * sizeof *a);
    if (a == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (u64 m = k - 1;; m++) {
        u128 c = 1;
        for (unsigned j = 0; j < k; j++) c = c * (m - j) / (j + 1);
        if (c >= limit) break;
        if (n == cap) {
            if (cap > SIZE_MAX / sizeof *a / 2) {
                fputs("binomial table is too large\n", stderr);
                free(a);
                exit(EXIT_FAILURE);
            }
            const size_t new_cap = cap * 2;
            u64 *grown = realloc(a, new_cap * sizeof *a);
            if (grown == NULL) {
                perror("realloc");
                free(a);
                exit(EXIT_FAILURE);
            }
            a = grown;
            cap = new_cap;
        }
        a[n++] = (u64)c;
    }
    *len = n;
    return a;
}

static u64 count(u64 n) {
    size_t n4, n6, n8;
    u64 *c4 = table(4, n, &n4), *c6 = table(6, n, &n6), *c8 = table(8, n, &n8);
    u64 r = 0;
    for (size_t z = 0; z < n8; z++) {
        u64 a = n - c8[z];
        for (size_t y = 0; y < n6 && c6[y] < a; y++) {
            u64 b = a - c6[y];
            for (size_t x = 0; x < n4 && c4[x] < b; x++)
                if (is_tri(b - c4[x])) r++;
        }
    }
    free(c4); free(c6); free(c8);
    return r;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        /* Validate every argument before starting any potentially long search. */
        for (int i = 1; i < argc; i++) {
            u64 n;
            if (!parse_n(argv[i], &n)) {
                fprintf(stderr, "invalid N '%s': expected decimal digits in 0..%" PRIu64 "\n",
                        argv[i], max_n);
                return EXIT_FAILURE;
            }
        }
        for (int i = 1; i < argc; i++) {
            u64 n = 0;
            (void)parse_n(argv[i], &n);
            printf("a(%" PRIu64 ") = %" PRIu64 "\n", n, count(n));
        }
    } else {
        for (u64 n = 1; n <= 80; n++)
            printf("%" PRIu64 "%s", count(n), n < 80 ? ", " : "\n");
        printf("a(23343989) = %" PRIu64 "\n", count(UINT64_C(23343989)));
        printf("a(896315812331399) = %" PRIu64 "\n", count(UINT64_C(896315812331399)));
    }
    if (fflush(stdout) == EOF || ferror(stdout)) {
        fputs("failed to write results to stdout\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
