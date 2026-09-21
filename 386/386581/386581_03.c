/*
 * A386581: Number of normal multisets of size n with no permutation
 *          having all distinct run lengths.
 *
 * 出力: b386581_03.txt（OEIS の b-file 形式 "n a(n)"）
 *
 * コンパイルと実行
 *   cc -O3 -std=c11 -Wall -Wextra -Wpedantic \
 *      386581_03.c -o 386581_03
 *   ./386581_03 [N]        # n = 0..N（既定 N = 100, 最大 127）
 *   ./386581_03 N --output FILE
 *   ./386581_03 N --no-bfile
 *
 *   目安: N = 100 は環境により数十秒・数百 MB を要する。N を 10 増やすごとに
 *   時間・メモリともおよそ 2.5〜3.5 倍になる。最大値 127 は表現上の上限であり、
 *   実用的な実行時間やメモリ量を保証するものではない。
 *
 * 補数列 A386580 との関係
 *   大きさ n >= 1 の正規多重集合は n の組成と一対一に対応し、全部で 2^(n-1) 個。
 *   したがって
 *
 *     A386581(n) = 2^(n-1) - A386580(n)  (n >= 1),
 *     A386581(0) = 0.
 *
 *   このファイル自身が下記の frontier DP で A386580(n) を計算して補数を取る。
 *   386580_03.c の関数を呼び出すことも、同ファイルをリンクすることもない。
 *
 * ---------------------------------------------------------------------------
 * 1. A386580 の計算：組成への帰着
 *   大きさ n の正規多重集合は、重複度の列 (m_1, ..., m_k) すなわち n の組成と
 *   一対一に対応する。条件は重複度の多重集合（分割 λ）だけで決まるので、
 *
 *     A386580(n) = Σ_{λ ⊢ n, λ が条件を満たす} ℓ(λ)! / Π_j m_j(λ)!
 *
 *   となる（各分割を、その並べ替えである組成の個数で重み付け）。
 *
 * 2. 判定条件
 *   λ = (m_1, ..., m_k) がランの長さのすべて異なる並べ替えをもつ ⇔
 *     各 m_i を相異なる部分に分割 π_i し、全 π_i の部分が互いに重ならない
 *     ように選べる。
 *   つまり λ は、{1, 2, ...} の有限部分集合 W を集合分割したときの
 *   各ブロックの和の多重集合として表せる。このような W を λ の「証拠」と呼ぶ。
 *
 *   （同じ値のランが隣接しないように並べる釣り合い条件 2 max r_i <= Σ r_i + 1
 *     は自動的に満たせる。ある値 x のランが r 個、他の値のランが計 O 個で
 *     r >= O + 2 とすると、x のランの長さの 2 つの和は 2r - 3 通り以上あり、
 *     衝突しうる長さは他の値の O 個と x 自身の r - 2 個しかない。
 *     2r - 3 > O + r - 2 なので、衝突しない 2 つを 1 つのランにまとめられる。
 *     これを繰り返せば r <= O + 1 にできる。）
 *
 * 3. run 長上限による 0/1 frontier DP
 *   R_x(n) を、{1,...,x} の部分集合を証拠にもつ、和 n の分割全体とする。
 *   R_0(0) は空分割だけで、その他は空である。x を使わない証拠はそのまま残り、
 *   x を使う証拠は R_{x-1}(n-x) の分割に対し、x を新しいブロックにするか、
 *   既存ブロックの一つへ足すことでちょうど得られる。したがって
 *
 *     R_x(n) = R_{x-1}(n) union T_x(R_{x-1}(n-x)).
 *
 *   実装は x を外側ループにし、和を降順に更新する。よって同じ更新中に作った
 *   状態を再利用せず、run 長 x は高々一度だけ使われる。x の処理後には R_x(x)
 *   が最終状態であり、その重み付き和が A386580(x) となる。
 *
 *   各和の frontier は独立した完全キーのハッシュ集合で保持する。和 n の集合は
 *   x=n の処理後には二度と更新されないため、その時点で密な配列へ圧縮する。
 *   ハッシュ衝突は u128 キー全体の比較で解決し、確率的な同一視は行わない。
 *
 * 値は 128 ビット整数で保持する（両数列とも 2^(n-1) 以下なので n <= 127 まで安全）。
 * 完了した各項は b-file の既存の連続 prefix と照合し、排他ロックの下で
 * 一時ファイルを fsync してから rename する。既存項は削除せず、計算途中の項も
 * 記録しない。複数プロセスが同じ b-file を更新しても重複や欠番を作らない。
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if !defined(__SIZEOF_INT128__)
#error "386581_03.c requires unsigned __int128"
#endif

__extension__ typedef unsigned __int128 u128;

#define MAXN 127
#define MAXL 15 /* 相異なる正整数 15 個の和は 120、16 個なら 136 > 127 */

/*
 * 分割の表現（u128）
 *   ビット 0..104 : 部分を降順に 7 ビットずつ（最大 15 個、0 は「なし」）
 *   ビット 127    : 常に 1（ハッシュ表で 0 を空きの印にするため）
 */
#define MARK (((u128)1) << 127)
#define U128_MAX_VALUE (~(u128)0)

_Static_assert(MAXL * (MAXL + 1) / 2 <= MAXN,
               "MAXL cannot represent every possible witness");
_Static_assert((MAXL + 1) * (MAXL + 2) / 2 > MAXN,
               "MAXL is larger than necessary");
_Static_assert(7 * MAXL < 127, "parts overlap the hash marker");

static const char *output_path = "b386581_03.txt";
static bool write_bfile = true;

static void u128_text(u128 value, char text[40]);

static const uint64_t known_terms[] = {
    UINT64_C(0), UINT64_C(0), UINT64_C(1), UINT64_C(1),
    UINT64_C(5), UINT64_C(11), UINT64_C(20), UINT64_C(51),
    UINT64_C(108), UINT64_C(229), UINT64_C(448), UINT64_C(953),
    UINT64_C(1940), UINT64_C(3951), UINT64_C(7986), UINT64_C(15972)
};

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void verify_known(int n, u128 value)
{
    const size_t count = sizeof(known_terms) / sizeof(known_terms[0]);
    if (n >= 0 && (size_t)n < count && value != known_terms[n]) {
        char text[40];
        u128_text(value, text);
        fprintf(stderr,
                "error: known-term mismatch at n=%d: got %s, "
                "expected %" PRIu64 "\n",
                n, text, known_terms[n]);
        exit(EXIT_FAILURE);
    }
}

static u128 encode(const int *p, int t)
{
    u128 k = MARK;
    for (int j = 0; j < t; j++)
        k |= (u128)p[j] << (7 * j);
    return k;
}

static int decode(u128 k, int *p)
{
    int t = 0;
    for (int j = 0; j < MAXL; j++) {
        int v = (int)((k >> (7 * j)) & 127);
        if (v == 0)
            break;
        p[t++] = v;
    }
    return t;
}

/* ---------------- 和ごとの frontier 集合 ---------------- */

typedef struct {
    u128 *entry;
    size_t capacity;
    size_t size;
    bool dense;
} StateSet;

static StateSet layer[MAXN + 1];
static u128 a386580_value[MAXN + 1];
static size_t stored_states;
static uint64_t transition_attempts;

static size_t hash_u128(u128 k)
{
    unsigned long long lo = (unsigned long long)k;
    unsigned long long hi = (unsigned long long)(k >> 64);
    unsigned long long h = lo * 0x9E3779B97F4A7C15ULL ^ (hi + 0x632BE59BD9B4E019ULL) * 0xC2B2AE3D27D4EB4FULL;
    h ^= h >> 29;
    h *= 0xBF58476D1CE4E5B9ULL;
    h ^= h >> 32;
    return (size_t)h;
}

static void *xcalloc(size_t n, size_t sz)
{
    if (sz != 0U && n > SIZE_MAX / sz)
        die("allocation size overflow");
    void *p = calloc(n, sz);
    if (!p)
        die("out of memory");
    return p;
}

static void *xrealloc(void *old, size_t n, size_t sz)
{
    if (sz != 0U && n > SIZE_MAX / sz)
        die("allocation size overflow");
    void *p = realloc(old, n * sz);
    if (!p)
        die("out of memory");
    return p;
}

static bool set_insert_raw(StateSet *set, u128 key)
{
    size_t i = hash_u128(key) & (set->capacity - 1U);
    while (set->entry[i] != 0U) {
        if (set->entry[i] == key)
            return false;
        i = (i + 1U) & (set->capacity - 1U);
    }
    set->entry[i] = key;
    ++set->size;
    return true;
}

static void set_grow(StateSet *set)
{
    const size_t old_capacity = set->capacity;
    u128 *old = set->entry;
    if (old_capacity != 0U && old_capacity > SIZE_MAX / 2U)
        die("frontier capacity overflow");
    set->capacity = old_capacity == 0U ? 16U : 2U * old_capacity;
    set->entry = xcalloc(set->capacity, sizeof(*set->entry));
    const size_t old_size = set->size;
    set->size = 0U;
    for (size_t i = 0U; i < old_capacity; ++i) {
        if (old[i] != 0U)
            (void)set_insert_raw(set, old[i]);
    }
    free(old);
    if (set->size != old_size)
        die("frontier rehash lost a state");
}

static bool set_insert(StateSet *set, u128 key)
{
    if (set->dense)
        die("attempted to update a completed frontier");
    if (set->size == SIZE_MAX)
        die("frontier size overflow");
    if (set->capacity == 0U ||
        set->size + 1U > set->capacity - set->capacity / 4U)
        set_grow(set);
    return set_insert_raw(set, key);
}

/* ---------------- 集計 ---------------- */

static unsigned long long fact[MAXL + 1];

/* 分割（降順の配列）の並べ替え（組成）の個数 ℓ(λ)! / Π m_j(λ)! */
static unsigned long long compositions_count(const int *p, int t)
{
    unsigned long long r = fact[t];
    int j = 0;
    while (j < t) {
        int k = j;
        while (k < t && p[k] == p[j])
            k++;
        r /= fact[k - j];
        j = k;
    }
    return r;
}

static void offer(int n, const int *p, int t)
{
    if (set_insert(&layer[n], encode(p, t))) {
        const u128 addend = compositions_count(p, t);
        if (a386580_value[n] > U128_MAX_VALUE - addend)
            die("A386580 value overflow");
        a386580_value[n] += addend;
        if (stored_states == SIZE_MAX)
            die("stored-state count overflow");
        ++stored_states;
    }
}

static void compact_layer(int n)
{
    StateSet *set = &layer[n];
    if (set->dense)
        return;
    if (set->size == 0U)
        die("cannot compact an empty completed frontier");
    u128 *dense = xrealloc(NULL, set->size, sizeof(*dense));
    size_t used = 0U;
    for (size_t i = 0U; i < set->capacity; ++i) {
        if (set->entry[i] != 0U)
            dense[used++] = set->entry[i];
    }
    if (used != set->size)
        die("frontier compaction lost a state");
    free(set->entry);
    set->entry = dense;
    set->capacity = set->size;
    set->dense = true;
}

static void extend_state(int target, int x, u128 key)
{
    int p[MAXL], q[MAXL + 1];
    const int t = decode(key, p);
    if (t < MAXL) {
        int nt = 0, done = 0;
        for (int j = 0; j < t; ++j) {
            if (!done && x >= p[j]) {
                q[nt++] = x;
                done = 1;
            }
            q[nt++] = p[j];
        }
        if (!done)
            q[nt++] = x;
        offer(target, q, nt);
    }

    for (int j = 0; j < t; ++j) {
        if (j > 0 && p[j] == p[j - 1])
            continue;
        const int combined = p[j] + x;
        int nt = 0, done = 0;
        for (int r = 0; r < t; ++r) {
            if (r == j)
                continue;
            if (!done && combined >= p[r]) {
                q[nt++] = combined;
                done = 1;
            }
            q[nt++] = p[r];
        }
        if (!done)
            q[nt++] = combined;
        offer(target, q, nt);
    }
}

static void advance_length(int x, int maximum_n)
{
    for (int sum = maximum_n - x; sum >= 0; --sum) {
        const StateSet *source = &layer[sum];
        const size_t limit = source->dense ? source->size : source->capacity;
        for (size_t i = 0U; i < limit; ++i) {
            const u128 key = source->entry == NULL ? 0U : source->entry[i];
            if (key == 0U)
                continue;
            if (transition_attempts == UINT64_MAX)
                die("transition statistic overflow");
            ++transition_attempts;
            extend_state(sum + x, x, key);
        }
    }
    compact_layer(x);
}

static size_t allocated_state_slots(void)
{
    size_t total = 0U;
    for (int n = 0; n <= MAXN; ++n) {
        if (total > SIZE_MAX - layer[n].capacity)
            die("allocated-slot statistic overflow");
        total += layer[n].capacity;
    }
    return total;
}

/* ---------------- 安全な b-file 入出力 ---------------- */

static void u128_text(u128 value, char text[40])
{
    char reverse[40];
    size_t length = 0U;
    do {
        reverse[length++] = (char)('0' + (unsigned)(value % 10U));
        value /= 10U;
    } while (value != 0U);
    for (size_t i = 0U; i < length; ++i)
        text[i] = reverse[length - 1U - i];
    text[length] = '\0';
}

static bool parse_u128_token(const char *text, const char **end, u128 *value)
{
    if (*text < '0' || *text > '9')
        return false;
    u128 result = 0U;
    do {
        const unsigned digit = (unsigned)(*text - '0');
        if (result > (U128_MAX_VALUE - digit) / 10U)
            return false;
        result = 10U * result + digit;
        ++text;
    } while (*text >= '0' && *text <= '9');
    *end = text;
    *value = result;
    return true;
}

static char *path_with_suffix(const char *suffix)
{
    const size_t path_length = strlen(output_path);
    const size_t suffix_length = strlen(suffix);
    if (path_length > SIZE_MAX - suffix_length - 1U)
        die("output path is too long");
    char *path = malloc(path_length + suffix_length + 1U);
    if (!path)
        die("cannot allocate derived output path");
    memcpy(path, output_path, path_length);
    memcpy(path + path_length, suffix, suffix_length + 1U);
    return path;
}

static int lock_bfile(void)
{
    char *path = path_with_suffix(".lock");
    const int descriptor = open(path, O_RDWR | O_CREAT, 0666);
    free(path);
    if (descriptor < 0)
        die("cannot open b-file lock");
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET
    };
    while (fcntl(descriptor, F_SETLKW, &lock) != 0) {
        if (errno != EINTR) {
            close(descriptor);
            die("cannot lock b-file");
        }
    }
    return descriptor;
}

static void unlock_bfile(int descriptor)
{
    struct flock lock = {
        .l_type = F_UNLCK,
        .l_whence = SEEK_SET
    };
    if (fcntl(descriptor, F_SETLK, &lock) != 0 || close(descriptor) != 0)
        die("cannot unlock b-file");
}

/* コメントと空行を除き、0 から始まる厳密な連続 prefix だけを受理する。 */
static unsigned read_bfile(u128 values[MAXN + 1])
{
    FILE *input = fopen(output_path, "r");
    if (!input) {
        if (errno == ENOENT)
            return 0U;
        die("cannot read b-file");
    }

    char line[256];
    unsigned next = 0U;
    while (fgets(line, sizeof(line), input)) {
        const size_t length = strlen(line);
        if (length == sizeof(line) - 1U && line[length - 1U] != '\n') {
            fclose(input);
            die("b-file contains an overlong line");
        }
        char *cursor = line;
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        if (*cursor == '\0' || *cursor == '\n' || *cursor == '#')
            continue;
        if (*cursor < '0' || *cursor > '9') {
            fclose(input);
            die("b-file contains an invalid index");
        }

        errno = 0;
        char *index_end = NULL;
        const uintmax_t parsed_index = strtoumax(cursor, &index_end, 10);
        if (errno == ERANGE || index_end == cursor || parsed_index > MAXN ||
            parsed_index != next) {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        cursor = index_end;
        if (*cursor != ' ' && *cursor != '\t') {
            fclose(input);
            die("b-file is malformed or has a gap");
        }
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;

        const char *value_end = NULL;
        u128 value;
        if (!parse_u128_token(cursor, &value_end, &value)) {
            fclose(input);
            die("b-file contains an invalid value");
        }
        cursor = (char *)value_end;
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        if (*cursor == '\r')
            ++cursor;
        if (*cursor == '\n')
            ++cursor;
        if (*cursor != '\0') {
            fclose(input);
            die("b-file contains trailing data");
        }
        values[next++] = value;
    }
    if (ferror(input) || fclose(input) != 0)
        die("cannot finish reading b-file");
    return next;
}

static void validate_bfile(void)
{
    if (!write_bfile)
        return;
    u128 values[MAXN + 1];
    const int descriptor = lock_bfile();
    (void)read_bfile(values);
    unlock_bfile(descriptor);
}

static void record_term(unsigned n, u128 value)
{
    if (!write_bfile)
        return;

    u128 values[MAXN + 1];
    const int lock_descriptor = lock_bfile();
    unsigned prefix = read_bfile(values);
    if (n < prefix) {
        if (values[n] != value) {
            unlock_bfile(lock_descriptor);
            die("computed term disagrees with the b-file");
        }
        unlock_bfile(lock_descriptor);
        return;
    }
    if (n != prefix) {
        unlock_bfile(lock_descriptor);
        die("b-file gap detected while recording");
    }
    values[prefix++] = value;

    char *temporary = path_with_suffix(".tmp.XXXXXX");
    const int temporary_descriptor = mkstemp(temporary);
    if (temporary_descriptor < 0) {
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot create temporary b-file");
    }
    FILE *output = fdopen(temporary_descriptor, "w");
    if (!output) {
        close(temporary_descriptor);
        unlink(temporary);
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot open temporary b-file stream");
    }

    bool failed = false;
    for (unsigned index = 0U; index < prefix; ++index) {
        char number[40];
        u128_text(values[index], number);
        if (fprintf(output, "%u %s\n", index, number) < 0)
            failed = true;
    }
    if (!failed && fflush(output) != 0)
        failed = true;
    if (!failed && fsync(fileno(output)) != 0)
        failed = true;
    if (fclose(output) != 0)
        failed = true;
    if (failed || rename(temporary, output_path) != 0) {
        unlink(temporary);
        unlock_bfile(lock_descriptor);
        free(temporary);
        die("cannot atomically update b-file");
    }
    free(temporary);
    unlock_bfile(lock_descriptor);
}

static int parse_n(const char *text)
{
    if (!text || *text == '\0')
        die("N is empty");
    errno = 0;
    char *end = NULL;
    const long value = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' ||
        value < 0 || value > MAXN) {
        fprintf(stderr, "error: N must be an integer in 0..%d: %s\n",
                MAXN, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static double monotonic_seconds(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        die("clock_gettime failed");
    return (double)value.tv_sec + 1.0e-9 * (double)value.tv_nsec;
}

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [N] [--output FILE|--no-bfile]\n"
            "N defaults to 100 and must be in 0..%d.\n"
            "Completed terms are atomically recorded in %s.\n",
            program, MAXN, output_path);
}

int main(int argc, char **argv)
{
    int nmax = 100;
    bool have_n = false;
    bool output_option = false;
    bool no_bfile_option = false;
    for (int argument = 1; argument < argc; ++argument) {
        const char *text = argv[argument];
        if (strcmp(text, "--help") == 0 || strcmp(text, "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(text, "--output") == 0) {
            if (output_option || argument + 1 >= argc ||
                argv[argument + 1][0] == '\0')
                die("invalid --output");
            output_option = true;
            output_path = argv[++argument];
        } else if (strcmp(text, "--no-bfile") == 0) {
            if (no_bfile_option)
                die("duplicate --no-bfile");
            no_bfile_option = true;
        } else {
            if (text[0] == '-' || have_n) {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            nmax = parse_n(text);
            have_n = true;
        }
    }
    if (output_option && no_bfile_option)
        die("--output and --no-bfile are mutually exclusive");
    write_bfile = !no_bfile_option;
    validate_bfile();

    fact[0] = 1U;
    for (int k = 1; k <= MAXL; ++k) {
        if (fact[k - 1] > ULLONG_MAX / (unsigned long long)k)
            die("factorial overflow");
        fact[k] = fact[k - 1] * (unsigned long long)k;
    }
    const double started = monotonic_seconds();
    offer(0, NULL, 0);
    compact_layer(0);
    if (a386580_value[0] != 1U)
        die("A386580(0) must equal 1");
    const u128 value0 = 0U;
    verify_known(0, value0);
    record_term(0U, value0);
    {
        char number[40];
        u128_text(value0, number);
        if (fprintf(stderr,
                    "0 %s  (%.2f s, witness partitions %zu, stored %zu, "
                    "transitions=%" PRIu64 ")\n",
                    number, monotonic_seconds() - started,
                    layer[0].size, stored_states, transition_attempts) < 0)
            die("cannot write progress");
    }

    for (int x = 1; x <= nmax; ++x) {
        advance_length(x, nmax);
        const u128 total = (u128)1U << (x - 1);
        if (a386580_value[x] > total)
            die("A386580 value exceeds the number of compositions");
        const u128 value = total - a386580_value[x];
        verify_known(x, value);
        record_term((unsigned)x, value);

        char number[40];
        u128_text(value, number);
        const size_t slots = allocated_state_slots();
        const double memory_mib =
            (double)slots * (double)sizeof(u128) / (1024.0 * 1024.0);
        if (fprintf(stderr,
                    "%d %s  (%.2f s, witness partitions %zu, stored %zu, "
                    "slots %.1f MiB, transitions=%" PRIu64 ")\n",
                    x, number, monotonic_seconds() - started,
                    layer[x].size, stored_states, memory_mib,
                    transition_attempts) < 0)
            die("cannot write progress");
    }

    for (int n = 0; n <= nmax; ++n)
        free(layer[n].entry);
    if (write_bfile) {
        if (fprintf(stderr, "updated %s\n", output_path) < 0)
            die("cannot write completion message");
    }
    return EXIT_SUCCESS;
}
