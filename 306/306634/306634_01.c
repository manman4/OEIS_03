/*
 * A306634: numbers k such that A215339(6*k+2) = 0.
 *
 * Let P(n) be the Perrin sequence A001608:
 *
 *     P(0)=3, P(1)=0, P(2)=2, P(n)=P(n-2)+P(n-3).
 *
 * A215339(n) is P(n) modulo n.  If k>0, then n=6*k+2 is an
 * even integer greater than 2 and hence composite.  Therefore
 *
 *     k is in A306634, k>0
 *       <=> n=6*k+2 is an unrestricted Perrin pseudoprime
 *       <=> n is in A013998 and n == 2 (mod 6).
 *
 * Thus the fast way to compute this exceptionally sparse sequence is to
 * filter A013998, not to test all k.  Up to the end of the current A013998
 * b-file this changes about 1.24e11 Perrin tests into 1702 integer tests.
 * k=0 is handled separately because n=2 is prime and is not in A013998.
 *
 * The --verify option independently checks every emitted source value by
 * computing P(n) modulo n in O(log n) time.  Binary powering is done in
 * Z/nZ[x]/(x^3-x-1): if x^n = c0+c1*x+c2*x^2, then
 *
 *     P(n) = 3*c0 + 2*c2 (mod n).
 *
 * --scan is a slower, source-independent exhaustive check over k.  It is
 * useful for small ranges and regression testing, but is deliberately not
 * the default algorithm: reaching the fourth known term that way requires
 * 123725995973 modular Perrin tests.
 *
 * Build:
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic 306634_01.c -o 306634_01
 *
 * Fast use with the official A013998 b-file:
 *   curl -fsSL https://oeis.org/A013998/b013998.txt |
 *       ./306634_01 --filter - --verify
 *   ./306634_01 --filter b013998.txt
 *
 * Other modes:
 *   ./306634_01 --scan 5000000
 *   ./306634_01 --check 123725995972
 *   ./306634_01 --self-test
 *
 * Output is in A306634 b-file form (offset 1): "index value".
 */

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__SIZEOF_INT128__)
#error "306634_01.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 u128;

typedef struct {
    uint64_t c[3];
} Poly;

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static uint64_t parse_u64(const char *text, const char *name)
{
    char *end = NULL;

    if (text == NULL || *text == '\0' || *text == '-') {
        fprintf(stderr, "error: invalid %s: %s\n", name,
                text == NULL ? "(null)" : text);
        exit(EXIT_FAILURE);
    }
    errno = 0;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' ||
        value > UINT64_MAX) {
        fprintf(stderr, "error: invalid %s: %s\n", name, text);
        exit(EXIT_FAILURE);
    }
    return (uint64_t)value;
}

static uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t modulus)
{
    return (uint64_t)(((u128)a * b) % modulus);
}

static uint64_t add_mod(uint64_t a, uint64_t b, uint64_t modulus)
{
    /* The arguments are reduced.  This form cannot overflow uint64_t. */
    return a >= modulus - b ? a - (modulus - b) : a + b;
}

static uint64_t add_product(uint64_t sum, uint64_t a, uint64_t b,
                            uint64_t modulus)
{
    return add_mod(sum, mul_mod(a, b, modulus), modulus);
}

/* Multiply modulo x^3-x-1 and modulo modulus. */
static Poly poly_multiply(Poly a, Poly b, uint64_t modulus)
{
    uint64_t d0 = mul_mod(a.c[0], b.c[0], modulus);
    uint64_t d1 = mul_mod(a.c[0], b.c[1], modulus);
    uint64_t d2 = mul_mod(a.c[0], b.c[2], modulus);
    uint64_t d3 = mul_mod(a.c[1], b.c[2], modulus);
    uint64_t d4 = mul_mod(a.c[2], b.c[2], modulus);

    d1 = add_product(d1, a.c[1], b.c[0], modulus);
    d2 = add_product(d2, a.c[1], b.c[1], modulus);
    d2 = add_product(d2, a.c[2], b.c[0], modulus);
    d3 = add_product(d3, a.c[2], b.c[1], modulus);

    /* x^3=x+1 and x^4=x^2+x. */
    Poly result;
    result.c[0] = add_mod(d0, d3, modulus);
    result.c[1] = add_mod(add_mod(d1, d3, modulus), d4, modulus);
    result.c[2] = add_mod(d2, d4, modulus);
    return result;
}

static uint64_t perrin_mod(uint64_t n, uint64_t modulus)
{
    if (modulus == 0) die("zero modulus in Perrin calculation");
    if (modulus == 1) return 0;

    Poly result = {{1, 0, 0}};
    Poly power = {{0, 1, 0}}; /* x */
    while (n != 0) {
        if ((n & 1) != 0) result = poly_multiply(result, power, modulus);
        n >>= 1;
        if (n != 0) power = poly_multiply(power, power, modulus);
    }

    uint64_t trace = mul_mod(3, result.c[0], modulus);
    trace = add_mod(trace, mul_mod(2, result.c[2], modulus), modulus);
    return trace;
}

static bool k_to_index(uint64_t k, uint64_t *n)
{
    if (k > (UINT64_MAX - 2) / 6) return false;
    *n = 6 * k + 2;
    return true;
}

static bool is_a306634(uint64_t k)
{
    uint64_t n;
    if (!k_to_index(k, &n)) die("6*k+2 overflows uint64_t");
    return perrin_mod(n, n) == 0;
}

static void print_term(uint64_t index, uint64_t value)
{
    if (printf("%" PRIu64 " %" PRIu64 "\n", index, value) < 0)
        die("cannot write output");
}

static void filter_a013998(const char *path, bool verify)
{
    FILE *input;
    if (strcmp(path, "-") == 0) {
        input = stdin;
    } else {
        input = fopen(path, "r");
        if (input == NULL) {
            fprintf(stderr, "error: cannot open %s: %s\n", path,
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    print_term(1, 0);
    uint64_t output_index = 2;
    uint64_t expected_source_index = 1;
    uint64_t previous = 0;
    char line[256];

    while (fgets(line, sizeof(line), input) != NULL) {
        const char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;
        char source_index_text[32], value_text[32], extra;
        const int fields = sscanf(p, "%31s %31s %c", source_index_text,
                                  value_text, &extra);
        if (fields != 2) die("malformed A013998 b-file line");

        const uint64_t source_index =
            parse_u64(source_index_text, "A013998 index");
        const uint64_t n = parse_u64(value_text, "A013998 value");
        if (source_index != expected_source_index)
            die("A013998 b-file has a gap or an out-of-order index");
        if (source_index != 1 && n <= previous)
            die("A013998 b-file values are not strictly increasing");
        ++expected_source_index;
        previous = n;

        if (verify && perrin_mod(n, n) != 0)
            die("an A013998 input value fails the Perrin congruence");
        if (n % 6 == 2) print_term(output_index++, (n - 2) / 6);
    }
    if (ferror(input)) die("cannot finish reading A013998 b-file");
    if (input != stdin && fclose(input) != 0)
        die("cannot close A013998 b-file");
    if (fflush(stdout) != 0) die("cannot flush output");
}

static void exhaustive_scan(uint64_t maximum_k)
{
    uint64_t index = 1;
    for (uint64_t k = 0;; ++k) {
        /* P(n) modulo 2 has period 7 and is zero only at 1,2,4 mod 7. */
        const unsigned n_mod_7 = (unsigned)((6 * (k % 7) + 2) % 7);
        if ((n_mod_7 == 1 || n_mod_7 == 2 || n_mod_7 == 4) &&
            is_a306634(k)) {
            print_term(index++, k);
        }
        if (k == maximum_k) break;
    }
    if (fflush(stdout) != 0) die("cannot flush output");
}

static void self_test(void)
{
    uint64_t a = 3, b = 0, c = 2;
    for (uint64_t n = 0; n <= 90; ++n) {
        static const uint64_t moduli[] = {
            2, 3, 5, 7, 11, 97, 121, 65537, UINT64_C(4294967291),
            UINT64_C(18446744073709551557)
        };
        for (size_t i = 0; i < sizeof(moduli) / sizeof(moduli[0]); ++i) {
            if (perrin_mod(n, moduli[i]) != a % moduli[i])
                die("Perrin binary-power self-test failed");
        }
        const uint64_t next = a + b;
        a = b;
        b = c;
        c = next;
    }

    static const uint64_t known_k[] = {
        0, UINT64_C(2755452), UINT64_C(4570452),
        UINT64_C(123725995972)
    };
    for (size_t i = 0; i < sizeof(known_k) / sizeof(known_k[0]); ++i) {
        if (!is_a306634(known_k[i]))
            die("known A306634 term fails verification");
    }
    static const uint64_t nonterms[] = {1, 2, 100, 2755451, 2755453};
    for (size_t i = 0; i < sizeof(nonterms) / sizeof(nonterms[0]); ++i) {
        if (is_a306634(nonterms[i]))
            die("known nonterm passes verification");
    }
    puts("ok: Perrin powering and known A306634 terms verified");
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "usage:\n"
            "  %s --filter FILE|- [--verify]\n"
            "  %s --scan MAX_K\n"
            "  %s --check K\n"
            "  %s --self-test\n",
            program, program, program, program);
}

int main(int argc, char **argv)
{
    if (argc >= 3 && strcmp(argv[1], "--filter") == 0) {
        bool verify = false;
        if (argc == 4 && strcmp(argv[3], "--verify") == 0) verify = true;
        else if (argc != 3) {
            usage(stderr, argv[0]);
            return EXIT_FAILURE;
        }
        filter_a013998(argv[2], verify);
        return EXIT_SUCCESS;
    }
    if (argc == 3 && strcmp(argv[1], "--scan") == 0) {
        exhaustive_scan(parse_u64(argv[2], "MAX_K"));
        return EXIT_SUCCESS;
    }
    if (argc == 3 && strcmp(argv[1], "--check") == 0) {
        const uint64_t k = parse_u64(argv[2], "K");
        uint64_t n;
        if (!k_to_index(k, &n)) die("6*k+2 overflows uint64_t");
        const uint64_t residue = perrin_mod(n, n);
        printf("k=%" PRIu64 ", 6*k+2=%" PRIu64
               ", P(6*k+2) mod (6*k+2)=%" PRIu64 ": %s\n",
               k, n, residue, residue == 0 ? "term" : "not a term");
        return EXIT_SUCCESS;
    }
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) {
        self_test();
        return EXIT_SUCCESS;
    }
    usage(stderr, argv[0]);
    return EXIT_FAILURE;
}
