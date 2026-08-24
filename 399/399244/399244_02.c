/*
 * A399244: number of set partitions of [n] into blocks whose element sums
 * are distinct primes.
 *
 * Let F(M,U) count completions of the remaining element mask M when U is
 * the set of prime sums already used.  Put the largest element of M in a
 * distinguished block B.  This fixes the order in which blocks are removed
 * and avoids counting a partition more than once:
 *
 *   F(0,U) = 1,
 *   F(M,U) = sum F(M \\ B, U union {sum(B)}),
 *
 * where B contains max(M), sum(B) is prime, and that prime is not in U.
 * The complete pair (M,U) is the memo key, so hash collisions cannot change
 * the result.  Unlike 399244_01, all prime-sum blocks are indexed once by
 * their largest element in compact Patricia subset tries.  A state walks
 * only trie branches compatible with its remaining element mask.
 * Root blocks are distributed through a dynamic pthread work queue.  All
 * workers share a concurrent full-key memo table; busy entries make duplicate
 * states wait, and decreasing element counts make those waits cycle-free.
 * The memo is sized before each term and is never resized while workers run.
 * At n=20 the memo, block index, and root tasks use about 180 MiB.
 *
 * A necessary condition for a state to succeed is that sum(M) be a subset
 * sum of the unused primes.  A small bitset subset-sum calculation checks
 * this condition on demand.  Counts, additions, allocations, and table
 * loads are checked.  Known terms are used only by --self-test after a
 * value has been computed.
 *
 * Build:
 *   clang -O3 -march=native -std=c11 -Wall -Wextra -Wpedantic \
 *       399244_02.c -o 399244_02 -pthread
 *
 * Examples:
 *   ./399244_02 --upto 13 --threads 8 --self-test --stats
 *   ./399244_02 --target 19 --threads 8 --self-test --stats
 *
 * Each computed result is also written to b399244_02.txt in the current
 * directory.  The file is flushed and synchronized after every term, so a
 * completed prefix remains available if a long --upto run is interrupted.
 */

#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_N 20U
#define DEFAULT_N 13U
#define KNOWN_N 19U
#define PARALLEL_MIN_N 12U
#define TASK_CHUNK 16U
#define MEMO_EMPTY 0U
#define MEMO_RESERVED 1U
#define MEMO_BUSY 2U
#define MEMO_COMPLETE 3U
#define LOW_BITS 16U
#define LOW_SIZE (UINT32_C(1) << LOW_BITS)
#define HIGH_BITS (MAX_N - LOW_BITS)
#define HIGH_SIZE (UINT32_C(1) << HIGH_BITS)
#define MAX_SUM (MAX_N * (MAX_N + 1U) / 2U)
#define REACH_WORDS ((MAX_SUM + 64U) / 64U)
#define TRIE_NONE UINT32_MAX
#define BFILE_NAME "b399244_02.txt"

typedef struct {
    _Atomic uint8_t *status;
    uint32_t *masks;
    uint64_t *used_primes;
    uint64_t *values;
    size_t capacity;
    _Atomic size_t size;
} ConcurrentMemo;

typedef struct {
    uint32_t child[2];
    uint32_t required;
    uint32_t leaf_count;
    uint64_t prime_mask;
} TrieNode;

typedef struct {
    TrieNode *nodes;
    uint32_t node_count;
    uint32_t root;
} BlockCatalog;

typedef struct {
    ConcurrentMemo memo;
    _Atomic uint64_t computed_states;
    _Atomic uint64_t nonzero_states;
    _Atomic uint64_t pruned_states;
    _Atomic uint64_t wait_count;
    unsigned thread_count;
    unsigned n;
    uint16_t low_sum[LOW_SIZE];
    uint16_t high_sum[HIGH_SIZE];
    uint64_t prime_bit[MAX_SUM + 1U];
    unsigned primes[64];
    unsigned prime_count;
    BlockCatalog catalog[MAX_N];
    unsigned catalog_count;
} Context;

typedef struct {
    Context *context;
} Worker;

typedef struct {
    size_t index;
} MemoToken;

typedef struct {
    uint32_t block_rest;
    uint64_t prime_mask;
} RootTask;

static const uint64_t known[KNOWN_N + 1U] = {
    UINT64_C(1), UINT64_C(0), UINT64_C(1), UINT64_C(0),
    UINT64_C(3), UINT64_C(4), UINT64_C(10), UINT64_C(24),
    UINT64_C(67), UINT64_C(145), UINT64_C(647), UINT64_C(2350),
    UINT64_C(8756), UINT64_C(33386), UINT64_C(102684),
    UINT64_C(611265), UINT64_C(2322253), UINT64_C(11765298),
    UINT64_C(58632351), UINT64_C(351652531)
};

static _Noreturn void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static unsigned parse_n(const char *text)
{
    errno = 0;
    char *end = NULL;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > MAX_N) {
        fprintf(stderr, "error: n must be in 0..%u: %s\n", MAX_N, text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static unsigned parse_threads(const char *text)
{
    errno = 0;
    char *end = NULL;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' ||
        value == 0 || value > 64U) {
        fprintf(stderr, "error: threads must be in 1..64: %s\n", text);
        exit(EXIT_FAILURE);
    }
    return (unsigned)value;
}

static unsigned default_thread_count(void)
{
    const long detected = sysconf(_SC_NPROCESSORS_ONLN);
    if (detected <= 0) return 1U;
    return detected > 8 ? 8U : (unsigned)detected;
}

static unsigned highest_bit_index(uint32_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return 31U - (unsigned)__builtin_clz(mask);
#else
    unsigned index = 0;
    while (mask >>= 1U) ++index;
    return index;
#endif
}

static unsigned bit_count(uint32_t mask)
{
#if defined(__clang__) || defined(__GNUC__)
    return (unsigned)__builtin_popcount(mask);
#else
    unsigned count = 0;
    while (mask != 0) {
        mask &= mask - 1U;
        ++count;
    }
    return count;
#endif
}

static uint16_t subset_sum(const Context *context, uint32_t mask)
{
    return (uint16_t)(context->low_sum[mask & (LOW_SIZE - 1U)] +
                      context->high_sum[mask >> LOW_BITS]);
}

static uint64_t hash_key(uint64_t key)
{
    key ^= key >> 30U;
    key *= UINT64_C(0xbf58476d1ce4e5b9);
    key ^= key >> 27U;
    key *= UINT64_C(0x94d049bb133111eb);
    key ^= key >> 31U;
    return key;
}

static uint64_t state_hash(uint32_t mask, uint64_t used_primes)
{
    return hash_key(used_primes ^
                    hash_key((uint64_t)mask + UINT64_C(0x9e3779b97f4a7c15)));
}

static bool memo_claim(Context *context, uint32_t mask,
                       uint64_t used_primes, uint64_t *value,
                       MemoToken *token)
{
    ConcurrentMemo *memo = &context->memo;
    size_t index =
        (size_t)state_hash(mask, used_primes) & (memo->capacity - 1U);
    for (size_t probes = 0; probes < memo->capacity; ++probes) {
        uint8_t status =
            atomic_load_explicit(&memo->status[index], memory_order_acquire);
        if (status == MEMO_RESERVED) {
            do {
                sched_yield();
                status = atomic_load_explicit(&memo->status[index],
                                              memory_order_acquire);
            } while (status == MEMO_RESERVED);
            continue;
        }
        if (status == MEMO_EMPTY) {
            uint8_t expected = MEMO_EMPTY;
            if (atomic_compare_exchange_weak_explicit(
                    &memo->status[index], &expected, MEMO_RESERVED,
                    memory_order_acq_rel, memory_order_acquire)) {
                memo->masks[index] = mask;
                memo->used_primes[index] = used_primes;
                atomic_store_explicit(&memo->status[index], MEMO_BUSY,
                                      memory_order_release);
                const size_t size =
                    atomic_fetch_add_explicit(&memo->size, 1,
                                              memory_order_relaxed) + 1U;
                if (size * 10U >= memo->capacity * 7U)
                    die("concurrent memo table exceeded 70% load");
                token->index = index;
                return true;
            }
            continue;
        }
        if (memo->masks[index] == mask &&
            memo->used_primes[index] == used_primes) {
            if (status == MEMO_BUSY) {
                atomic_fetch_add_explicit(&context->wait_count, 1,
                                          memory_order_relaxed);
                unsigned spins = 0;
                do {
                    status = atomic_load_explicit(&memo->status[index],
                                                  memory_order_acquire);
                    if (++spins == 1024U && status == MEMO_BUSY) {
                        sched_yield();
                        spins = 0;
                    }
                } while (status == MEMO_BUSY);
            }
            if (status != MEMO_COMPLETE)
                die("invalid concurrent memo state");
            *value = memo->values[index];
            return false;
        }
        index = (index + 1U) & (memo->capacity - 1U);
    }
    die("concurrent memo table is full");
}

static void memo_publish(Context *context, const MemoToken *token,
                         uint64_t value)
{
    context->memo.values[token->index] = value;
    atomic_store_explicit(&context->memo.status[token->index],
                          MEMO_COMPLETE, memory_order_release);
    atomic_fetch_add_explicit(&context->computed_states, 1,
                              memory_order_relaxed);
    if (value != 0)
        atomic_fetch_add_explicit(&context->nonzero_states, 1,
                                  memory_order_relaxed);
}

static bool unused_primes_can_sum(const Context *context,
                                  uint64_t used_primes, unsigned sum)
{
    uint64_t reachable[REACH_WORDS];
    memset(reachable, 0, sizeof(reachable));
    reachable[0] = UINT64_C(1);
    for (unsigned i = 0; i < context->prime_count; ++i) {
        const unsigned prime = context->primes[i];
        if (prime > sum) break;
        if ((used_primes & (UINT64_C(1) << i)) != 0) continue;
        const unsigned word_shift = prime / 64U;
        const unsigned bit_shift = prime % 64U;
        for (unsigned word = REACH_WORDS; word-- > 0;) {
            const uint64_t source = reachable[word];
            if (source == 0) continue;
            const unsigned target = word + word_shift;
            if (target < REACH_WORDS)
                reachable[target] |= source << bit_shift;
            if (bit_shift != 0 && target + 1U < REACH_WORDS)
                reachable[target + 1U] |=
                    source >> (64U - bit_shift);
        }
    }
    return ((reachable[sum / 64U] >> (sum % 64U)) & UINT64_C(1)) != 0;
}

static void checked_add(uint64_t *total, uint64_t addend)
{
    if (UINT64_MAX - *total < addend)
        die("uint64_t overflow while counting partitions");
    *total += addend;
}

typedef struct {
    Context *context;
    BlockCatalog *catalog;
    const uint32_t *masks;
    unsigned pivot_value;
    uint32_t used;
} CatalogBuilder;

static uint32_t build_catalog_node(CatalogBuilder *builder,
                                   size_t begin, size_t end)
{
    if (begin >= end || builder->used >= builder->catalog->node_count)
        die("internal prime-block trie construction error");
    const uint32_t index = builder->used++;
    TrieNode *node = &builder->catalog->nodes[index];
    if (end - begin == 1U) {
        node->child[0] = TRIE_NONE;
        node->child[1] = TRIE_NONE;
        node->required = builder->masks[begin];
        node->leaf_count = 1U;
        const unsigned sum = builder->pivot_value +
            subset_sum(builder->context, node->required);
        node->prime_mask = builder->context->prime_bit[sum];
        if (node->prime_mask == 0)
            die("nonprime leaf in prime-block trie");
        return index;
    }

    const uint32_t difference =
        builder->masks[begin] ^ builder->masks[end - 1U];
    if (difference == 0) die("duplicate mask in prime-block trie");
    const uint32_t split =
        UINT32_C(1) << highest_bit_index(difference);
    size_t low = begin;
    size_t high = end;
    while (low < high) {
        const size_t middle = low + (high - low) / 2U;
        if ((builder->masks[middle] & split) == 0)
            low = middle + 1U;
        else
            high = middle;
    }
    if (low == begin || low == end)
        die("internal prime-block trie split error");
    node->child[0] = build_catalog_node(builder, begin, low);
    node->child[1] = build_catalog_node(builder, low, end);
    const TrieNode *left = &builder->catalog->nodes[node->child[0]];
    const TrieNode *right = &builder->catalog->nodes[node->child[1]];
    node->required = left->required & right->required;
    if (UINT32_MAX - left->leaf_count < right->leaf_count)
        die("prime-block trie leaf count overflow");
    node->leaf_count = left->leaf_count + right->leaf_count;
    node->prime_mask = 0;
    return index;
}

static void build_catalog(Context *context, unsigned pivot_index)
{
    BlockCatalog *catalog = &context->catalog[pivot_index];
    const uint32_t subset_count = UINT32_C(1) << pivot_index;
    const unsigned pivot_value = pivot_index + 1U;
    size_t valid_count = 0;
    for (uint32_t mask = 0; mask < subset_count; ++mask) {
        const unsigned sum = pivot_value + subset_sum(context, mask);
        if (context->prime_bit[sum] != 0) ++valid_count;
    }
    if (valid_count == 0) {
        catalog->root = TRIE_NONE;
        return;
    }
    if (valid_count > UINT32_MAX / 2U ||
        valid_count > SIZE_MAX / sizeof(uint32_t))
        die("prime-block catalog is too large");
    uint32_t *masks = malloc(valid_count * sizeof(*masks));
    if (masks == NULL) die("could not allocate prime-block masks");
    size_t used = 0;
    for (uint32_t mask = 0; mask < subset_count; ++mask) {
        const unsigned sum = pivot_value + subset_sum(context, mask);
        if (context->prime_bit[sum] != 0) masks[used++] = mask;
    }
    if (used != valid_count) die("prime-block catalog size mismatch");
    const size_t node_count = valid_count * 2U - 1U;
    if (node_count > UINT32_MAX ||
        node_count > SIZE_MAX / sizeof(*catalog->nodes))
        die("prime-block trie is too large");
    catalog->nodes = malloc(node_count * sizeof(*catalog->nodes));
    if (catalog->nodes == NULL)
        die("could not allocate prime-block trie");
    catalog->node_count = (uint32_t)node_count;
    CatalogBuilder builder =
        {context, catalog, masks, pivot_value, 0U};
    catalog->root = build_catalog_node(&builder, 0, valid_count);
    free(masks);
    if (builder.used != catalog->node_count || catalog->root != 0)
        die("internal prime-block trie size mismatch");
    if (pivot_value >= 18U) {
        fprintf(stderr,
                "preparing n=%u: prime-block index %zu blocks, %u nodes\n",
                pivot_value, valid_count, catalog->node_count);
        if (fflush(stderr) == EOF)
            die("could not flush block-index preparation output");
    }
}

static uint64_t count_partitions(Worker *worker, uint32_t mask,
                                 uint64_t used_primes, unsigned depth);

static void add_catalog_partitions(Worker *worker,
                                   const BlockCatalog *catalog,
                                   uint32_t node_index, uint32_t rest,
                                   uint64_t used_primes, unsigned depth,
                                   uint64_t *total)
{
    const TrieNode *node = &catalog->nodes[node_index];
    if ((node->required & ~rest) != 0) return;
    if (node->child[0] == TRIE_NONE) {
        if ((used_primes & node->prime_mask) == 0)
            checked_add(total,
                        count_partitions(worker, rest ^ node->required,
                                         used_primes | node->prime_mask,
                                         depth + 1U));
        return;
    }
    add_catalog_partitions(worker, catalog, node->child[1], rest,
                           used_primes, depth, total);
    add_catalog_partitions(worker, catalog, node->child[0], rest,
                           used_primes, depth, total);
}

static uint64_t count_partitions(Worker *worker, uint32_t mask,
                                 uint64_t used_primes, unsigned depth)
{
    if (mask == 0) return 1U;
    if (depth >= MAX_N) die("internal recursion depth overflow");
    Context *context = worker->context;

    uint64_t memoized;
    MemoToken token;
    if (!memo_claim(context, mask, used_primes, &memoized, &token))
        return memoized;

    const unsigned remaining_sum = subset_sum(context, mask);
    if (!unused_primes_can_sum(context, used_primes, remaining_sum)) {
        atomic_fetch_add_explicit(&context->pruned_states, 1,
                                  memory_order_relaxed);
        memo_publish(context, &token, 0U);
        return 0U;
    }

    const unsigned pivot_index = highest_bit_index(mask);
    const uint32_t pivot = UINT32_C(1) << pivot_index;
    const uint32_t rest = mask ^ pivot;
    uint64_t total = 0;
    const BlockCatalog *catalog = &context->catalog[pivot_index];
    if (catalog->root != TRIE_NONE)
        add_catalog_partitions(worker, catalog, catalog->root, rest,
                               used_primes, depth, &total);

    memo_publish(context, &token, total);
    return total;
}

static void initialize_element_sums(Context *context)
{
    context->low_sum[0] = 0;
    for (uint32_t mask = 1; mask < LOW_SIZE; ++mask) {
        const uint32_t bit = mask & (0U - mask);
        context->low_sum[mask] =
            (uint16_t)(context->low_sum[mask ^ bit] +
                       highest_bit_index(bit) + 1U);
    }

    context->high_sum[0] = 0;
    for (uint32_t mask = 1; mask < HIGH_SIZE; ++mask) {
        const uint32_t bit = mask & (0U - mask);
        context->high_sum[mask] =
            (uint16_t)(context->high_sum[mask ^ bit] + LOW_BITS +
                       highest_bit_index(bit) + 1U);
    }
}

static void initialize_primes(Context *context)
{
    bool is_prime[MAX_SUM + 1U];
    memset(is_prime, 0, sizeof(is_prime));
    for (unsigned value = 2; value <= MAX_SUM; ++value)
        is_prime[value] = true;
    for (unsigned divisor = 2; divisor <= MAX_SUM / divisor; ++divisor) {
        if (!is_prime[divisor]) continue;
        for (unsigned composite = divisor * divisor;
             composite <= MAX_SUM; composite += divisor)
            is_prime[composite] = false;
    }

    for (unsigned value = 2; value <= MAX_SUM; ++value) {
        if (!is_prime[value]) continue;
        if (context->prime_count == 64U)
            die("more than 64 prime sums require a wider used-prime mask");
        const unsigned index = context->prime_count++;
        context->primes[index] = value;
        context->prime_bit[value] = UINT64_C(1) << index;
    }

}

static size_t memo_capacity_for_n(unsigned n)
{
    unsigned bits;
    if (n <= 13U)
        bits = 16U;
    else if (n <= 16U)
        bits = 18U;
    else
        bits = n + 3U;
    return (size_t)1U << bits;
}

static void context_prepare_n(Context *context, unsigned n)
{
    const size_t capacity = memo_capacity_for_n(n);
    if (capacity > SIZE_MAX / sizeof(*context->memo.status) ||
        capacity > SIZE_MAX / sizeof(*context->memo.masks) ||
        capacity > SIZE_MAX / sizeof(*context->memo.used_primes) ||
        capacity > SIZE_MAX / sizeof(*context->memo.values))
        die("concurrent memo allocation size overflow");
    free(context->memo.status);
    free(context->memo.masks);
    free(context->memo.used_primes);
    free(context->memo.values);
    context->memo.status = calloc(capacity, sizeof(*context->memo.status));
    context->memo.masks = malloc(capacity * sizeof(*context->memo.masks));
    context->memo.used_primes =
        malloc(capacity * sizeof(*context->memo.used_primes));
    context->memo.values = malloc(capacity * sizeof(*context->memo.values));
    if (context->memo.status == NULL || context->memo.masks == NULL ||
        context->memo.used_primes == NULL || context->memo.values == NULL)
        die("could not allocate concurrent memo table");
    context->memo.capacity = capacity;
    atomic_store_explicit(&context->memo.size, 0, memory_order_relaxed);
    while (context->catalog_count < n) {
        build_catalog(context, context->catalog_count);
        ++context->catalog_count;
    }
    context->n = n;
    if (n >= 18U) {
        fprintf(stderr, "preparing n=%u: fresh memo with %zu slots\n",
                n, capacity);
        if (fflush(stderr) == EOF)
            die("could not flush memo preparation output");
    }
}

static Context *context_create(unsigned thread_count)
{
    Context *context = calloc(1, sizeof(*context));
    if (context == NULL) die("could not allocate the DP context");
    context->thread_count = thread_count;
    initialize_element_sums(context);
    initialize_primes(context);
    return context;
}

static void worker_destroy(Worker *worker)
{
    (void)worker;
}

static void context_destroy(Context *context)
{
    for (unsigned i = 0; i < context->catalog_count; ++i)
        free(context->catalog[i].nodes);
    free(context->memo.status);
    free(context->memo.masks);
    free(context->memo.used_primes);
    free(context->memo.values);
    free(context);
}

typedef struct {
    Context *context;
    RootTask *tasks;
    size_t task_count;
    uint32_t rest;
    _Atomic size_t next_task;
    Worker *workers;
    uint64_t *results;
} RootQueue;

typedef struct {
    RootQueue *queue;
    unsigned worker_index;
} ThreadArgument;

static void fill_root_tasks(const BlockCatalog *catalog,
                            uint32_t node_index, RootTask *tasks,
                            size_t *used)
{
    const TrieNode *node = &catalog->nodes[node_index];
    if (node->child[0] == TRIE_NONE) {
        tasks[*used].block_rest = node->required;
        tasks[*used].prime_mask = node->prime_mask;
        ++*used;
        return;
    }
    fill_root_tasks(catalog, node->child[1], tasks, used);
    fill_root_tasks(catalog, node->child[0], tasks, used);
}

static RootTask *build_root_tasks(const Context *context, uint32_t mask,
                                  size_t *result_count)
{
    const unsigned pivot_index = highest_bit_index(mask);
    const uint32_t pivot = UINT32_C(1) << pivot_index;
    if ((mask ^ pivot) != pivot - 1U)
        die("root mask is not an initial interval");
    const BlockCatalog *catalog = &context->catalog[pivot_index];
    if (catalog->root == TRIE_NONE) {
        *result_count = 0;
        return NULL;
    }
    const size_t total = catalog->nodes[catalog->root].leaf_count;
    if (total > SIZE_MAX / sizeof(RootTask))
        die("root task allocation size overflow");
    RootTask *tasks = malloc(total * sizeof(*tasks));
    if (tasks == NULL)
        die("could not allocate root task list");
    size_t used = 0;
    fill_root_tasks(catalog, catalog->root, tasks, &used);
    if (used != total) die("internal root task count mismatch");
    *result_count = total;
    return tasks;
}

static void *root_worker(void *opaque)
{
    ThreadArgument *argument = opaque;
    RootQueue *queue = argument->queue;
    Worker *worker = &queue->workers[argument->worker_index];
    uint64_t total = 0;
    for (;;) {
        const size_t begin =
            atomic_fetch_add_explicit(&queue->next_task, TASK_CHUNK,
                                      memory_order_relaxed);
        if (begin >= queue->task_count) break;
        size_t end = begin + TASK_CHUNK;
        if (end > queue->task_count) end = queue->task_count;
        for (size_t i = begin; i < end; ++i) {
            const RootTask task = queue->tasks[i];
            checked_add(&total,
                        count_partitions(worker,
                                         queue->rest ^ task.block_rest,
                                         task.prime_mask, 0U));
        }
    }
    queue->results[argument->worker_index] = total;
    return NULL;
}

static uint64_t count_full(Context *context, uint32_t mask)
{
    if (mask == 0) return 1U;
    Worker serial_worker;
    memset(&serial_worker, 0, sizeof(serial_worker));
    serial_worker.context = context;
    const unsigned term = bit_count(mask);
    if (term < PARALLEL_MIN_N || context->thread_count == 1U) {
        const uint64_t result =
            count_partitions(&serial_worker, mask, 0U, 0U);
        worker_destroy(&serial_worker);
        return result;
    }

    uint64_t memoized;
    MemoToken root_token;
    if (!memo_claim(context, mask, 0U, &memoized, &root_token)) {
        worker_destroy(&serial_worker);
        return memoized;
    }
    if (!unused_primes_can_sum(context, 0U, subset_sum(context, mask))) {
        atomic_fetch_add_explicit(&context->pruned_states, 1,
                                  memory_order_relaxed);
        memo_publish(context, &root_token, 0U);
        worker_destroy(&serial_worker);
        return 0U;
    }

    size_t task_count = 0;
    RootTask *tasks = build_root_tasks(context, mask, &task_count);
    unsigned thread_count = context->thread_count;
    if (task_count != 0 && thread_count > task_count)
        thread_count = (unsigned)task_count;
    if (thread_count == 0) thread_count = 1U;
    RootQueue queue;
    memset(&queue, 0, sizeof(queue));
    queue.context = context;
    queue.tasks = tasks;
    queue.task_count = task_count;
    queue.rest = mask ^ (UINT32_C(1) << highest_bit_index(mask));
    queue.workers = calloc(thread_count, sizeof(*queue.workers));
    queue.results = calloc(thread_count, sizeof(*queue.results));
    ThreadArgument *arguments =
        calloc(thread_count, sizeof(*arguments));
    pthread_t *threads = thread_count > 1U
        ? calloc(thread_count - 1U, sizeof(*threads)) : NULL;
    if (queue.workers == NULL || queue.results == NULL ||
        arguments == NULL || (thread_count > 1U && threads == NULL))
        die("could not allocate root worker data");
    for (unsigned i = 0; i < thread_count; ++i) {
        queue.workers[i].context = context;
        arguments[i].queue = &queue;
        arguments[i].worker_index = i;
    }
    for (unsigned i = 1; i < thread_count; ++i) {
        const int error =
            pthread_create(&threads[i - 1U], NULL, root_worker,
                           &arguments[i]);
        if (error != 0) {
            fprintf(stderr, "error: pthread_create: %s\n",
                    strerror(error));
            exit(EXIT_FAILURE);
        }
    }
    root_worker(&arguments[0]);
    for (unsigned i = 1; i < thread_count; ++i) {
        const int error = pthread_join(threads[i - 1U], NULL);
        if (error != 0) {
            fprintf(stderr, "error: pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    uint64_t result = 0;
    for (unsigned i = 0; i < thread_count; ++i) {
        checked_add(&result, queue.results[i]);
        worker_destroy(&queue.workers[i]);
    }
    memo_publish(context, &root_token, result);
    free(threads);
    free(arguments);
    free(queue.results);
    free(queue.workers);
    free(tasks);
    worker_destroy(&serial_worker);
    return result;
}

static FILE *open_bfile(void)
{
    FILE *stream = fopen(BFILE_NAME, "w");
    if (stream == NULL) {
        fprintf(stderr, "error: could not create %s: %s\n",
                BFILE_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return stream;
}

static void write_bfile_term(FILE *stream, unsigned n, uint64_t value)
{
    if (fprintf(stream, "%u %" PRIu64 "\n", n, value) < 0 ||
        fflush(stream) != 0) {
        fprintf(stderr, "error: could not write %s: %s\n",
                BFILE_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    const int descriptor = fileno(stream);
    if (descriptor < 0 || fsync(descriptor) != 0) {
        fprintf(stderr, "error: could not synchronize %s: %s\n",
                BFILE_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void close_bfile(FILE *stream)
{
    if (fclose(stream) != 0) {
        fprintf(stderr, "error: could not close %s: %s\n",
                BFILE_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [--upto N | --target N] [--threads T] "
            "[--self-test] [--stats]\n"
            "  --upto N     print a(0)..a(N), 0 <= N <= %u (default %u)\n"
            "  --target N   print only a(N)\n"
            "               n=20 uses about 180 MiB including its index\n"
            "  --threads T  use 1..64 workers (default: up to 8 CPUs)\n"
            "  --self-test  compare computed terms with regression values\n"
            "  --stats      report memoized and pruned state counts\n"
            "  Results are synchronized to " BFILE_NAME
            " after every term.\n"
            "  --help       show this help\n",
            program, MAX_N, DEFAULT_N);
}

int main(int argc, char **argv)
{
    unsigned n = DEFAULT_N;
    unsigned thread_count = default_thread_count();
    bool target_only = false;
    bool self_test = false;
    bool stats = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--upto") == 0) {
            if (++i == argc) die("--upto requires an argument");
            n = parse_n(argv[i]);
            target_only = false;
        } else if (strcmp(argv[i], "--target") == 0) {
            if (++i == argc) die("--target requires an argument");
            n = parse_n(argv[i]);
            target_only = true;
        } else if (strcmp(argv[i], "--threads") == 0) {
            if (++i == argc) die("--threads requires an argument");
            thread_count = parse_threads(argv[i]);
        } else if (strcmp(argv[i], "--self-test") == 0) {
            self_test = true;
        } else if (strcmp(argv[i], "--stats") == 0) {
            stats = true;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "error: unknown argument: %s\n", argv[i]);
            usage(stderr, argv[0]);
            return EXIT_FAILURE;
        }
    }

    Context *context = context_create(thread_count);
    FILE *bfile = open_bfile();
    const unsigned first = target_only ? n : 0U;
    for (unsigned k = first; k <= n; ++k) {
        context_prepare_n(context, k);
        const uint32_t mask =
            k == 0 ? 0U : (UINT32_C(1) << k) - 1U;
        const uint64_t value = count_full(context, mask);
        if (self_test && k <= KNOWN_N && value != known[k]) {
            fprintf(stderr,
                    "self-test failed at n=%u: got %" PRIu64
                    ", expected %" PRIu64 "\n",
                    k, value, known[k]);
            close_bfile(bfile);
            context_destroy(context);
            return EXIT_FAILURE;
        }
        write_bfile_term(bfile, k, value);
        printf("%u %" PRIu64 "\n", k, value);
        if (fflush(stdout) == EOF) die("could not flush sequence output");
    }
    if (self_test) {
        if (target_only && n > KNOWN_N) {
            fprintf(stderr,
                    "no stored regression value at n=%u "
                    "(known values end at n=%u)\n",
                    n, KNOWN_N);
        } else {
            const unsigned tested = n < KNOWN_N ? n : KNOWN_N;
            fprintf(stderr, "self-test passed through known n=%u\n",
                    tested);
        }
    }
    if (stats)
        fprintf(stderr,
                "memoized states: %" PRIu64 ", nonzero: %" PRIu64
                ", pruned: %" PRIu64 ", waits: %" PRIu64
                ", table: %zu/%zu\n",
                atomic_load_explicit(&context->computed_states,
                                     memory_order_relaxed),
                atomic_load_explicit(&context->nonzero_states,
                                     memory_order_relaxed),
                atomic_load_explicit(&context->pruned_states,
                                     memory_order_relaxed),
                atomic_load_explicit(&context->wait_count,
                                     memory_order_relaxed),
                atomic_load_explicit(&context->memo.size,
                                     memory_order_relaxed),
                context->memo.capacity);
    close_bfile(bfile);
    context_destroy(context);
    return EXIT_SUCCESS;
}
