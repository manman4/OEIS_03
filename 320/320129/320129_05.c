#define _POSIX_C_SOURCE 200809L

/* A320129: number of ways to group 1..2n into n pairs whose sums are
 * pairwise distinct.
 *
 * Inclusion-exclusion over missed elements gives
 *
 *   a(n) = sum_{A subset of [2n]} (-1)^|A|
 *          e_n(c_3(A), ..., c_{4n-1}(A)),
 *
 * where c_s(A) is the number of pairs {x,y}, x+y=s, that avoid A, and e_n
 * is the elementary symmetric polynomial of degree n.  Gray code updates
 * c_s after one element changes, and reflection x -> 2n+1-x halves the
 * expensive evaluations of e_n.
 *
 * Arithmetic is modulo 2^64, or modulo 2^128 when compiled with -DW128.
 * The accepted ranges below ensure that the final answer is exact.  Partial
 * values from disjoint slices must be added modulo the selected power of 2.
 *
 * Build (GCC or Clang):
 *   cc -O3 -std=c11 -march=native 320129_05.c -o 320129_05
 *   cc -O3 -std=c11 -march=native -DW128 320129_05.c -o 320129_05_128
 *
 * Add -fopenmp when OpenMP is available.
 *
 * Run:
 *   ./320129_05_128 --term n [--split k]
 *   ./320129_05_128 --term n --split k --slice chunk_lo chunk_hi
 *   ./320129_05_128 --upto n
 *
 * The second form computes chunks in the half-open interval
 * [chunk_lo,chunk_hi).  A complete distributed run must cover
 * [0,2^split) exactly once.
 * --upto writes b320129_5.txt beside the executable.  While it is running,
 * completed terms are kept in b320129_5_part.txt.
 */

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#if defined(_MSC_VER)
#include <intrin.h>
#define POPCNT64(x) ((int)__popcnt64(x))
static int ctz64(uint64_t x)
{
    unsigned long i;
    (void)_BitScanForward64(&i, x);
    return (int)i;
}
#else
#define POPCNT64(x) ((int)__builtin_popcountll(x))
static int ctz64(uint64_t x)
{
    return __builtin_ctzll(x);
}
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#if defined(W128) && !defined(_MSC_VER)

typedef unsigned __int128 Acc;
static Acc acc_zero(void) { return (Acc)0; }
#define ACC_MULADD(d, v, s) ((d) += (Acc)(v) * (s))
#define ACC_ADDW(d, w, s)   ((d) += (Acc)(w) * (s))
#define ACC_SUBW(d, w, s)   ((d) -= (Acc)(w) * (s))
#define ACC_HI(a)           ((uint64_t)((a) >> 64))
#define ACC_LO(a)           ((uint64_t)(a))
#define ACC_ISZERO(a)       ((a) == 0)
#define ACC_DIVMOD10(a, r)  ((r) = (int)((a) % 10), (a) /= 10)
#define ACC_SET1(a)         ((a) = 1)

#elif defined(W128)

/* MSVC has no unsigned __int128, so retain the low 128 bits explicitly. */
typedef struct { uint64_t lo, hi; } Acc;

static Acc acc_zero(void)
{
    Acc z = {0, 0};
    return z;
}

static Acc acc_muladd(Acc d, uint64_t v, Acc s)
{
    uint64_t hi;
    uint64_t lo = _umul128(s.lo, v, &hi);
    unsigned char carry;

    hi += s.hi * v;
    carry = _addcarry_u64(0, d.lo, lo, &d.lo);
    (void)_addcarry_u64(carry, d.hi, hi, &d.hi);
    return d;
}

static Acc acc_sub(Acc d, Acc s)
{
    unsigned char borrow = _subborrow_u64(0, d.lo, s.lo, &d.lo);
    (void)_subborrow_u64(borrow, d.hi, s.hi, &d.hi);
    return d;
}

static Acc acc_add(Acc d, Acc s)
{
    unsigned char carry = _addcarry_u64(0, d.lo, s.lo, &d.lo);
    (void)_addcarry_u64(carry, d.hi, s.hi, &d.hi);
    return d;
}

static Acc acc_scale(uint64_t w, Acc s)
{
    return acc_muladd(acc_zero(), w, s);
}

static int acc_divmod10(Acc *a)
{
    uint64_t rem = a->hi % 10;
    uint64_t q1 = a->hi / 10;
    uint64_t q0 = 0;
    int b;

    for (b = 63; b >= 0; --b) {
        rem = (rem << 1) | ((a->lo >> b) & UINT64_C(1));
        if (rem >= 10) {
            rem -= 10;
            q0 |= UINT64_C(1) << b;
        }
    }
    a->hi = q1;
    a->lo = q0;
    return (int)rem;
}

#define ACC_MULADD(d, v, s) ((d) = acc_muladd((d), (uint64_t)(v), (s)))
#define ACC_ADDW(d, w, s)   ((d) = acc_add((d), acc_scale((uint64_t)(w), (s))))
#define ACC_SUBW(d, w, s)   ((d) = acc_sub((d), acc_scale((uint64_t)(w), (s))))
#define ACC_HI(a)           ((a).hi)
#define ACC_LO(a)           ((a).lo)
#define ACC_ISZERO(a)       ((a).lo == 0 && (a).hi == 0)
#define ACC_DIVMOD10(a, r)  ((r) = acc_divmod10(&(a)))
#define ACC_SET1(a)         ((a).lo = 1, (a).hi = 0)

#else

typedef uint64_t Acc;
static Acc acc_zero(void) { return UINT64_C(0); }
#define ACC_MULADD(d, v, s) ((d) += (Acc)(v) * (s))
#define ACC_ADDW(d, w, s)   ((d) += (Acc)(w) * (s))
#define ACC_SUBW(d, w, s)   ((d) -= (Acc)(w) * (s))
#define ACC_HI(a)           UINT64_C(0)
#define ACC_LO(a)           ((uint64_t)(a))
#define ACC_ISZERO(a)       ((a) == 0)
#define ACC_DIVMOD10(a, r)  ((r) = (int)((a) % 10), (a) /= 10)
#define ACC_SET1(a)         ((a) = 1)

#endif

#ifdef W128
enum { MAX_N = 28 };
#else
/* The known A320129 values through n=18 fit in 64 bits. */
enum { MAX_N = 18 };
#endif

static double now_seconds(void)
{
#ifdef _OPENMP
    return omp_get_wtime();
#else
    return (double)clock() / (double)CLOCKS_PER_SEC;
#endif
}

static int parse_ll(const char *text, long long *value)
{
    char *end;
    long long parsed;

    if (text == NULL || *text == '\0') return 0;
    errno = 0;
    end = NULL;
    parsed = strtoll(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0') return 0;
    *value = parsed;
    return 1;
}

static char *path_beside_executable(const char *argv0, const char *filename)
{
    char executable[4096];
    int found = 0;

#if defined(_WIN32)
    DWORD length = GetModuleFileNameA(NULL, executable,
                                      (DWORD)sizeof(executable));
    if (length > 0 && length < (DWORD)sizeof(executable)) found = 1;
#elif defined(__APPLE__)
    uint32_t size = (uint32_t)sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) == 0) found = 1;
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", executable,
                              sizeof(executable) - 1U);
    if (length >= 0) {
        executable[(size_t)length] = '\0';
        found = 1;
    }
#endif

    if (!found) {
        size_t length = strlen(argv0);
        if (length >= sizeof(executable)) return NULL;
        memcpy(executable, argv0, length + 1U);
    }

    {
        const char *slash = strrchr(executable, '/');
#if defined(_WIN32)
        const char *backslash = strrchr(executable, '\\');
        if (backslash != NULL && (slash == NULL || backslash > slash))
            slash = backslash;
#endif
        size_t directory_length =
            (slash == NULL) ? 1U : (size_t)(slash - executable);
        size_t filename_length = strlen(filename);
        char *path;

        if (slash != NULL && directory_length == 0U) directory_length = 1U;
        if (directory_length > SIZE_MAX - filename_length - 2U) return NULL;
        path = (char *)malloc(directory_length + filename_length + 2U);
        if (path == NULL) return NULL;

        if (slash == NULL) {
            path[0] = '.';
        } else if (slash == executable) {
            path[0] = '/';
        } else {
            memcpy(path, executable, directory_length);
        }
        path[directory_length] = '/';
        memcpy(path + directory_length + 1U, filename,
               filename_length + 1U);
        return path;
    }
}

static int flush_b_file(FILE *stream)
{
    if (fflush(stream) != 0) return 0;
#if defined(_WIN32)
    return _commit(_fileno(stream)) == 0;
#else
    return fsync(fileno(stream)) == 0;
#endif
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage:\n"
            "  %s --term N [--split K]\n"
            "  %s --term N --split K --slice LO HI\n"
            "  %s --upto N\n"
            "\n"
            "options:\n"
            "  --term N       compute only a(N)\n"
            "  --upto N       print a(0)..a(N) and write b320129_5.txt\n"
            "  --split K      divide the subset space into 2^K chunks\n"
            "                 (default: min(6, 2*N))\n"
            "  --slice LO HI  compute the half-open chunk interval [LO,HI)\n"
            "                 (requires --split)\n"
            "  --help         show this help\n"
            "\n"
            "examples:\n"
            "  %s --term 15\n"
            "  %s --upto 10\n"
            "  %s --term 15 --split 10\n"
            "  %s --term 15 --split 10 --slice 0 8\n",
            program, program, program, program, program, program, program);
}

static int write_acc(FILE *stream, Acc value)
{
    char decimal[64];
    int length = 0;
    int digit;

    if (ACC_ISZERO(value)) decimal[length++] = '0';
    while (!ACC_ISZERO(value)) {
        ACC_DIVMOD10(value, digit);
        decimal[length++] = (char)('0' + digit);
    }
    while (length > 0) {
        if (fputc(decimal[--length], stream) == EOF) return 0;
    }
    return 1;
}

static Acc compute_term(int n, int split, long long chunk_lo,
                        long long chunk_hi)
{
    int N = 2 * n;
    int smax = 4 * n - 1;
    int low_bits = N - split;
    Acc answer = acc_zero();

#pragma omp parallel
    {
        Acc e[MAX_N + 1];
        int c[4 * MAX_N + 1];
        Acc local_answer = acc_zero();
        long long chunk;

#pragma omp for schedule(dynamic, 1) nowait
        for (chunk = chunk_lo; chunk < chunk_hi; ++chunk) {
            uint64_t base = (uint64_t)chunk << low_bits;
            uint64_t subset = base;
            uint64_t reflected = 0;
            uint64_t full_mask = (UINT64_C(1) << N) - UINT64_C(1);
            uint64_t step;
            int nonzero = 0;
            int i;
            int s;
            int x;

            for (i = 0; i < N; ++i) {
                if ((subset >> i) & UINT64_C(1))
                    reflected |= UINT64_C(1) << (N - 1 - i);
            }

            for (s = 3; s <= smax; ++s) c[s] = 0;
            for (x = 1; x <= N; ++x) {
                int y;
                if ((subset >> (x - 1)) & UINT64_C(1)) continue;
                for (y = x + 1; y <= N; ++y) {
                    if (!((subset >> (y - 1)) & UINT64_C(1))) ++c[x + y];
                }
            }
            for (s = 3; s <= smax; ++s) {
                if (c[s] != 0) ++nonzero;
            }

            for (step = 0;; ++step) {
                if (nonzero >= n && reflected >= subset) {
                    uint64_t weight = (reflected == subset) ? 1 : 2;

                    memset(e, 0, ((size_t)n + 1U) * sizeof(*e));
                    ACC_SET1(e[0]);
                    for (s = 3; s <= smax; ++s) {
                        uint64_t count = (uint64_t)c[s];
                        if (count == 0) continue;
                        for (i = n; i >= 1; --i)
                            ACC_MULADD(e[i], count, e[i - 1]);
                    }
                    if (POPCNT64(subset) & 1)
                        ACC_SUBW(local_answer, weight, e[n]);
                    else
                        ACC_ADDW(local_answer, weight, e[n]);
                }

                if (step + UINT64_C(1) == (UINT64_C(1) << low_bits)) break;

                {
                    int v = ctz64(step + UINT64_C(1)) + 1;
                    int remove_pairs =
                        !((subset >> (v - 1)) & UINT64_C(1));
                    uint64_t surviving;

                    subset ^= UINT64_C(1) << (v - 1);
                    reflected ^= UINT64_C(1) << (N - v);
                    surviving = (~subset) & full_mask &
                                ~(UINT64_C(1) << (v - 1));

                    while (surviving != 0) {
                        int y = ctz64(surviving) + 1;
                        int sum = v + y;
                        int old = c[sum];
                        surviving &= surviving - UINT64_C(1);
                        if (remove_pairs) {
                            c[sum] = old - 1;
                            if (old == 1) --nonzero;
                        } else {
                            c[sum] = old + 1;
                            if (old == 0) ++nonzero;
                        }
                    }
                }
            }
        }

#pragma omp critical
        ACC_ADDW(answer, 1, local_answer);
    }

    return answer;
}

int main(int argc, char **argv)
{
    long long input_n = 0;
    long long input_split = 0;
    long long chunk_lo = 0;
    long long chunk_hi = 0;
    long long chunks;
    int mode = 0;                 /* 1: --term, 2: --upto */
    int have_split = 0;
    int have_slice = 0;
    int argi;

    for (argi = 1; argi < argc; ++argi) {
        if (strcmp(argv[argi], "--help") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[argi], "--term") == 0 ||
            strcmp(argv[argi], "--upto") == 0) {
            int new_mode = (strcmp(argv[argi], "--term") == 0) ? 1 : 2;
            if (mode != 0 || argi + 1 >= argc ||
                !parse_ll(argv[++argi], &input_n)) {
                fprintf(stderr, "error: --term and --upto each require one integer\n");
                return EXIT_FAILURE;
            }
            mode = new_mode;
        } else if (strcmp(argv[argi], "--split") == 0) {
            if (have_split || argi + 1 >= argc ||
                !parse_ll(argv[++argi], &input_split)) {
                fprintf(stderr, "error: --split requires one integer\n");
                return EXIT_FAILURE;
            }
            have_split = 1;
        } else if (strcmp(argv[argi], "--slice") == 0) {
            if (have_slice || argi + 2 >= argc ||
                !parse_ll(argv[++argi], &chunk_lo) ||
                !parse_ll(argv[++argi], &chunk_hi)) {
                fprintf(stderr, "error: --slice requires two integers LO HI\n");
                return EXIT_FAILURE;
            }
            have_slice = 1;
        } else {
            fprintf(stderr, "error: unknown argument: %s\n", argv[argi]);
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (mode == 0) {
        fprintf(stderr, "error: either --term N or --upto N is required\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (input_n < 0 || input_n > MAX_N) {
        fprintf(stderr, "error: N must be an integer in 0..%d\n", MAX_N);
        return EXIT_FAILURE;
    }
    if (mode == 2 && (have_split || have_slice)) {
        fprintf(stderr, "error: --upto cannot be combined with --split or --slice\n");
        return EXIT_FAILURE;
    }

    if (mode == 2) {
        int n;
        double start = now_seconds();
        char *final_path =
            path_beside_executable(argv[0], "b320129_5.txt");
        char *part_path =
            path_beside_executable(argv[0], "b320129_5_part.txt");
        FILE *bfile;

        if (final_path == NULL || part_path == NULL) {
            fprintf(stderr, "error: cannot construct the b-file path\n");
            free(final_path);
            free(part_path);
            return EXIT_FAILURE;
        }
        bfile = fopen(part_path, "w");
        if (bfile == NULL) {
            fprintf(stderr, "error: cannot open %s: %s\n",
                    part_path, strerror(errno));
            free(final_path);
            free(part_path);
            return EXIT_FAILURE;
        }

        for (n = 0; n <= (int)input_n; ++n) {
            Acc answer;

            if (n == 0) {
                answer = acc_zero();
                ACC_SET1(answer);
            } else {
                int N = 2 * n;
                int split = (N < 6) ? N : 6;
                long long all_chunks =
                    (long long)(UINT64_C(1) << split);
                answer = compute_term(n, split, 0, all_chunks);
            }

            if (fprintf(bfile, "%d ", n) < 0 ||
                !write_acc(bfile, answer) || fputc('\n', bfile) == EOF ||
                !flush_b_file(bfile)) {
                fprintf(stderr, "error: cannot write %s: %s\n",
                        part_path, strerror(errno));
                (void)fclose(bfile);
                free(final_path);
                free(part_path);
                return EXIT_FAILURE;
            }
            if (printf("%d ", n) < 0 || !write_acc(stdout, answer) ||
                putchar('\n') == EOF || fflush(stdout) != 0) {
                fprintf(stderr, "error: cannot write standard output\n");
                (void)fclose(bfile);
                free(final_path);
                free(part_path);
                return EXIT_FAILURE;
            }
        }
        if (fclose(bfile) != 0) {
            fprintf(stderr, "error: cannot close %s: %s\n",
                    part_path, strerror(errno));
            free(final_path);
            free(part_path);
            return EXIT_FAILURE;
        }
        if (rename(part_path, final_path) != 0) {
            fprintf(stderr, "error: cannot rename %s to %s: %s\n",
                    part_path, final_path, strerror(errno));
            free(final_path);
            free(part_path);
            return EXIT_FAILURE;
        }
        fprintf(stderr, "wrote %s (n=0..%lld)\n", final_path, input_n);
        fprintf(stderr, "%.1fs\n", now_seconds() - start);
        free(final_path);
        free(part_path);
        return EXIT_SUCCESS;
    }

    {
        int n = (int)input_n;
        int N;
        int split;
        double start;
        Acc answer;

        if (n == 0) {
            if (have_split || have_slice) {
                fprintf(stderr, "error: split arguments are not valid for n=0\n");
                return EXIT_FAILURE;
            }
            puts("a(0) = 1");
            return EXIT_SUCCESS;
        }

        N = 2 * n;
        if (have_split) {
            if (input_split < 1 || input_split > N) {
                fprintf(stderr, "error: split must be an integer in 1..%d\n", N);
                return EXIT_FAILURE;
            }
            split = (int)input_split;
        } else {
            split = (N < 6) ? N : 6;
        }

        chunks = (long long)(UINT64_C(1) << split);
        if (!have_slice) chunk_hi = chunks;
        if (have_slice) {
            if (!have_split) {
                fprintf(stderr, "error: --slice requires an explicit --split K\n");
                return EXIT_FAILURE;
            }
            if (chunk_lo < 0 || chunk_lo > chunk_hi || chunk_hi > chunks) {
                fprintf(stderr,
                        "error: require 0 <= LO <= HI <= %lld\n", chunks);
                return EXIT_FAILURE;
            }
        }

        start = now_seconds();
        answer = compute_term(n, split, chunk_lo, chunk_hi);

        if (chunk_lo != 0 || chunk_hi != chunks) {
            printf("PARTIAL %d %lld %lld %016llx%016llx\n",
                   n, chunk_lo, chunk_hi,
                   (unsigned long long)ACC_HI(answer),
                   (unsigned long long)ACC_LO(answer));
        } else {
            printf("a(%d) = ", n);
            if (!write_acc(stdout, answer)) {
                fprintf(stderr, "error: cannot write standard output\n");
                return EXIT_FAILURE;
            }
            putchar('\n');
        }
        fprintf(stderr, "%.1fs\n", now_seconds() - start);
    }
    return EXIT_SUCCESS;
}
