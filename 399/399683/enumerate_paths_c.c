#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Exact search for AR(n, P_k), 3 <= k <= n, without using the published
 * formula.  Edge colorings are restricted-growth strings, hence set
 * partitions of E(K_n) up to renaming colors.
 *
 * The search safely prunes a subtree when every still-relevant k either
 * already has a fully assigned rainbow P_k or cannot improve the incumbent
 * even if every remaining edge receives a new color.
 */

enum {
    MAX_N = 7,
    MAX_EDGES = MAX_N * (MAX_N - 1) / 2,
    /* There are 6825 undirected labeled paths of orders 3,...,7 in K_7. */
    MAX_PATHS = 7000,
};

_Static_assert(MAX_EDGES <= 32, "color masks require at most 32 edges");
_Static_assert(MAX_PATHS <= UINT16_MAX, "path indices must fit in uint16_t");

static const char *const DEFAULT_OUTPUT =
    "results_paths_c.json";

typedef struct {
    uint8_t u;
    uint8_t v;
} Edge;

typedef struct {
    uint8_t k;
    uint8_t edge_count;
    uint8_t edges[MAX_N - 1];
} Path;

typedef struct {
    int n;
    int edge_count;
    Edge edges[MAX_EDGES];
    int edge_index[MAX_N][MAX_N];

    Path paths[MAX_PATHS];
    int path_count;
    uint16_t completed_paths[MAX_EDGES][MAX_PATHS];
    int completed_path_count[MAX_EDGES];

    uint8_t coloring[MAX_EDGES];
    int best[MAX_N + 1];
    uint8_t witness[MAX_N + 1][MAX_EDGES];

    uint64_t nodes;
    uint64_t terminal_colorings;
    uint64_t pruned_subtrees;
    uint64_t progress_every;
    uint64_t next_progress;
} Search;

typedef struct {
    int n;
    int edge_count;
    int best[MAX_N + 1];
    uint8_t witness[MAX_N + 1][MAX_EDGES];
    uint64_t nodes;
    uint64_t terminal_colorings;
    uint64_t pruned_subtrees;
} Result;

typedef struct {
    int min_n;
    int max_n;
    const char *output;
    uint64_t progress_every;
    bool force;
} Options;

static void fail_errno(const char *operation);

static volatile sig_atomic_t stop_requested = 0;

static void request_stop(int signal_number)
{
    (void)signal_number;
    stop_requested = 1;
}

static void install_signal_handlers(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = request_stop;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, NULL) != 0 ||
        sigaction(SIGTERM, &action, NULL) != 0) {
        fail_errno("sigaction");
    }
}

static void fail(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

static void fail_errno(const char *operation)
{
    fprintf(stderr, "error: %s: %s\n", operation, strerror(errno));
    exit(EXIT_FAILURE);
}

static uint64_t parse_uint64(const char *text, const char *option)
{
    char *end = NULL;
    errno = 0;
    unsigned long long value = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') {
        fprintf(stderr, "error: invalid value for %s: %s\n", option, text);
        exit(EXIT_FAILURE);
    }
    return (uint64_t)value;
}

static int parse_int(const char *text, const char *option)
{
    uint64_t value = parse_uint64(text, option);
    if (value > INT_MAX) {
        fprintf(stderr, "error: value for %s is too large: %s\n", option, text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [--min-n N] [--max-n N] [--output PATH]\n"
            "          [--progress-every N] [--force]\n\n"
            "Exact formula-free search for AR(n, P_k), 3 <= k <= n <= %d.\n",
            program, MAX_N);
}

static Options parse_options(int argc, char **argv)
{
    Options options = {
        .min_n = 3,
        .max_n = MAX_N,
        .output = DEFAULT_OUTPUT,
        .progress_every = UINT64_C(10000000),
        .force = false,
    };

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--min-n") == 0 ||
            strcmp(argv[i], "--max-n") == 0 ||
            strcmp(argv[i], "--output") == 0 ||
            strcmp(argv[i], "--progress-every") == 0) {
            if (++i >= argc) {
                fail("option requires a value");
            }
            const char *option = argv[i - 1];
            if (strcmp(option, "--min-n") == 0) {
                options.min_n = parse_int(argv[i], option);
            } else if (strcmp(option, "--max-n") == 0) {
                options.max_n = parse_int(argv[i], option);
            } else if (strcmp(option, "--output") == 0) {
                options.output = argv[i];
            } else {
                options.progress_every = parse_uint64(argv[i], option);
            }
        } else if (strcmp(argv[i], "--force") == 0) {
            options.force = true;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(stdout, argv[0]);
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "error: unknown option: %s\n", argv[i]);
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (options.min_n < 3 || options.min_n > options.max_n ||
        options.max_n > MAX_N) {
        fprintf(stderr, "error: require 3 <= min-n <= max-n <= %d\n", MAX_N);
        exit(EXIT_FAILURE);
    }
    return options;
}

static void initialize_edges(Search *search)
{
    for (int u = 0; u < MAX_N; ++u) {
        for (int v = 0; v < MAX_N; ++v) {
            search->edge_index[u][v] = -1;
        }
    }

    int index = 0;
    for (int u = 0; u < search->n; ++u) {
        for (int v = u + 1; v < search->n; ++v) {
            search->edges[index] = (Edge){(uint8_t)u, (uint8_t)v};
            search->edge_index[u][v] = index;
            search->edge_index[v][u] = index;
            ++index;
        }
    }
    search->edge_count = index;
}

static void store_path(Search *search, int k, const int order[MAX_N])
{
    if (order[0] > order[k - 1]) {
        return;
    }
    if (search->path_count >= MAX_PATHS) {
        fail("internal path capacity exceeded");
    }

    int path_index = search->path_count++;
    Path *path = &search->paths[path_index];
    path->k = (uint8_t)k;
    path->edge_count = (uint8_t)(k - 1);
    int completion_edge = -1;
    for (int i = 0; i < k - 1; ++i) {
        int edge = search->edge_index[order[i]][order[i + 1]];
        if (edge < 0) {
            fail("internal edge lookup failure");
        }
        path->edges[i] = (uint8_t)edge;
        if (edge > completion_edge) {
            completion_edge = edge;
        }
    }

    int bucket_size = search->completed_path_count[completion_edge];
    if (bucket_size >= MAX_PATHS) {
        fail("internal completed-path capacity exceeded");
    }
    search->completed_paths[completion_edge][bucket_size] =
        (uint16_t)path_index;
    search->completed_path_count[completion_edge] = bucket_size + 1;
}

static void generate_paths_recursive(Search *search, int k, int depth,
                                     unsigned used_vertices,
                                     int order[MAX_N])
{
    if (depth == k) {
        store_path(search, k, order);
        return;
    }
    for (int vertex = 0; vertex < search->n; ++vertex) {
        unsigned bit = 1U << vertex;
        if ((used_vertices & bit) != 0U) {
            continue;
        }
        order[depth] = vertex;
        generate_paths_recursive(search, k, depth + 1,
                                 used_vertices | bit, order);
    }
}

static void initialize_paths(Search *search)
{
    int order[MAX_N] = {0};
    for (int k = 3; k <= search->n; ++k) {
        generate_paths_recursive(search, k, 0, 0U, order);
    }
}

static bool path_is_rainbow(const Search *search, const Path *path)
{
    uint32_t used_colors = 0U;
    for (int i = 0; i < path->edge_count; ++i) {
        uint32_t bit = 1U << search->coloring[path->edges[i]];
        if ((used_colors & bit) != 0U) {
            return false;
        }
        used_colors |= bit;
    }
    return true;
}

static uint32_t remove_completed_rainbow_paths(const Search *search,
                                                int assigned_edge,
                                                uint32_t active)
{
    int count = search->completed_path_count[assigned_edge];
    for (int i = 0; i < count && active != 0U; ++i) {
        const Path *path =
            &search->paths[search->completed_paths[assigned_edge][i]];
        uint32_t bit = 1U << (path->k - 3);
        if ((active & bit) != 0U && path_is_rainbow(search, path)) {
            active &= ~bit;
        }
    }
    return active;
}

static uint32_t remove_non_improving(const Search *search, int number_of_colors,
                                     int remaining_edges, uint32_t active)
{
    int maximum_possible = number_of_colors + remaining_edges;
    for (int k = 3; k <= search->n; ++k) {
        uint32_t bit = 1U << (k - 3);
        if ((active & bit) != 0U && maximum_possible <= search->best[k]) {
            active &= ~bit;
        }
    }
    return active;
}

static void print_progress(Search *search)
{
    if (search->progress_every == 0 || search->nodes < search->next_progress) {
        return;
    }
    printf("n=%d: nodes=%" PRIu64 ", terminal=%" PRIu64 ", current",
           search->n, search->nodes, search->terminal_colorings);
    for (int k = 3; k <= search->n; ++k) {
        printf(" AR(n,P_%d)=%d", k, search->best[k]);
    }
    putchar('\n');
    fflush(stdout);
    do {
        search->next_progress += search->progress_every;
    } while (search->next_progress <= search->nodes);
}

static void search_colorings(Search *search, int next_edge,
                             int number_of_colors, uint32_t active)
{
    if (stop_requested) {
        return;
    }
    ++search->nodes;
    print_progress(search);

    if (next_edge == search->edge_count) {
        ++search->terminal_colorings;
        for (int k = 3; k <= search->n; ++k) {
            uint32_t bit = 1U << (k - 3);
            if ((active & bit) != 0U && number_of_colors > search->best[k]) {
                search->best[k] = number_of_colors;
                memcpy(search->witness[k], search->coloring,
                       (size_t)search->edge_count);
            }
        }
        return;
    }

    /* Try a new color first so strong incumbents are usually found early. */
    for (int pass = 0; pass < 2; ++pass) {
        int first = pass == 0 ? number_of_colors : 0;
        int limit = pass == 0 ? number_of_colors + 1 : number_of_colors;
        for (int color = first; color < limit; ++color) {
            search->coloring[next_edge] = (uint8_t)color;
            int child_color_count =
                number_of_colors + (color == number_of_colors ? 1 : 0);
            uint32_t child_active = remove_completed_rainbow_paths(
                search, next_edge, active);
            int remaining = search->edge_count - next_edge - 1;
            child_active = remove_non_improving(
                search, child_color_count, remaining, child_active);
            if (child_active == 0U) {
                ++search->pruned_subtrees;
                continue;
            }
            search_colorings(search, next_edge + 1, child_color_count,
                             child_active);
            if (stop_requested) {
                return;
            }
        }
    }
}

static Result run_search(int n, uint64_t progress_every)
{
    Search search;
    memset(&search, 0, sizeof(search));
    search.n = n;
    search.progress_every = progress_every;
    search.next_progress = progress_every;
    initialize_edges(&search);
    initialize_paths(&search);

    /* The one-coloring is a direct witness for every k >= 3. */
    for (int k = 3; k <= n; ++k) {
        search.best[k] = 1;
        memset(search.witness[k], 0, (size_t)search.edge_count);
    }

    search.coloring[0] = 0;
    uint32_t active = (1U << (n - 2)) - 1U;
    active = remove_non_improving(
        &search, 1, search.edge_count - 1, active);
    search_colorings(&search, 1, 1, active);

    Result result;
    memset(&result, 0, sizeof(result));
    result.n = n;
    result.edge_count = search.edge_count;
    result.nodes = search.nodes;
    result.terminal_colorings = search.terminal_colorings;
    result.pruned_subtrees = search.pruned_subtrees;
    for (int k = 3; k <= n; ++k) {
        result.best[k] = search.best[k];
        memcpy(result.witness[k], search.witness[k],
               (size_t)search.edge_count);
    }

    printf("n=%d: paths=%d, nodes=%" PRIu64 ", terminal=%" PRIu64
           ", pruned=%" PRIu64,
           n, search.path_count, result.nodes, result.terminal_colorings,
           result.pruned_subtrees);
    for (int k = 3; k <= n; ++k) {
        printf(", AR(n,P_%d)=%d", k, result.best[k]);
    }
    putchar('\n');
    return result;
}

static void write_color_classes(FILE *stream, const Result *result, int k)
{
    fputs("[", stream);
    for (int color = 0; color < result->best[k]; ++color) {
        if (color != 0) {
            fputs(", ", stream);
        }
        fputs("[", stream);
        bool first_edge = true;
        int edge_index = 0;
        for (int u = 0; u < result->n; ++u) {
            for (int v = u + 1; v < result->n; ++v, ++edge_index) {
                if (result->witness[k][edge_index] != color) {
                    continue;
                }
                if (!first_edge) {
                    fputs(", ", stream);
                }
                fprintf(stream, "[%d, %d]", u, v);
                first_edge = false;
            }
        }
        fputs("]", stream);
    }
    fputs("]", stream);
}

static bool write_json_stream(FILE *stream, const Result *results,
                              int result_count)
{
    if (fprintf(stream,
                "{\n"
                "  \"problem\": 1105,\n"
                "  \"object\": \"AR(n, P_k) for 3 <= k <= n\",\n"
                "  \"method\": \"formula-free restricted-growth search "
                "with incremental rainbow-path and incumbent-bound "
                "pruning\",\n"
                "  \"values\": [\n") < 0) {
        return false;
    }

    for (int i = 0; i < result_count; ++i) {
        const Result *result = &results[i];
        if (i != 0) {
            fputs(",\n", stream);
        }
        fprintf(stream,
                "    {\n"
                "      \"n\": %d,\n"
                "      \"edge_count\": %d,\n"
                "      \"search_nodes\": %" PRIu64 ",\n"
                "      \"terminal_colorings\": %" PRIu64 ",\n"
                "      \"pruned_subtrees\": %" PRIu64 ",\n"
                "      \"entries\": [\n",
                result->n, result->edge_count, result->nodes,
                result->terminal_colorings, result->pruned_subtrees);
        for (int k = 3; k <= result->n; ++k) {
            if (k != 3) {
                fputs(",\n", stream);
            }
            fprintf(stream,
                    "        {\"k\": %d, \"anti_ramsey\": %d, "
                    "\"witness_color_classes\": ",
                    k, result->best[k]);
            write_color_classes(stream, result, k);
            fputs("}", stream);
        }
        fputs("\n      ]\n    }", stream);
    }

    fputs("\n  ],\n  \"flattened_terms\": [", stream);
    bool first_term = true;
    for (int i = 0; i < result_count; ++i) {
        for (int k = 3; k <= results[i].n; ++k) {
            if (!first_term) {
                fputs(", ", stream);
            }
            fprintf(stream, "%d", results[i].best[k]);
            first_term = false;
        }
    }
    fputs("]\n}\n", stream);
    return !ferror(stream);
}

static void write_results_atomic(const char *path, const Result *results,
                                 int result_count, bool force)
{
    if (!force && access(path, F_OK) == 0) {
        fprintf(stderr,
                "error: output already exists: %s; pass --force to replace it\n",
                path);
        exit(EXIT_FAILURE);
    }

    size_t template_size = strlen(path) + strlen(".tmp.XXXXXX") + 1;
    if (template_size > PATH_MAX) {
        fail("output path is too long");
    }
    char temporary_path[PATH_MAX];
    int length = snprintf(temporary_path, sizeof(temporary_path),
                          "%s.tmp.XXXXXX", path);
    if (length < 0 || (size_t)length >= sizeof(temporary_path)) {
        fail("could not construct temporary output path");
    }

    int descriptor = mkstemp(temporary_path);
    if (descriptor < 0) {
        fail_errno("mkstemp");
    }
    FILE *stream = fdopen(descriptor, "w");
    if (stream == NULL) {
        int saved_errno = errno;
        close(descriptor);
        unlink(temporary_path);
        errno = saved_errno;
        fail_errno("fdopen");
    }

    bool ok = write_json_stream(stream, results, result_count);
    if (ok && fflush(stream) != 0) {
        ok = false;
    }
    if (ok && fsync(fileno(stream)) != 0) {
        ok = false;
    }
    if (fclose(stream) != 0) {
        ok = false;
    }
    if (!ok) {
        int saved_errno = errno;
        unlink(temporary_path);
        errno = saved_errno;
        fail_errno("writing output");
    }
    if (rename(temporary_path, path) != 0) {
        int saved_errno = errno;
        unlink(temporary_path);
        errno = saved_errno;
        fail_errno("rename output");
    }
}

static void check_output_before_search(const char *path, bool force)
{
    if (force) {
        return;
    }
    errno = 0;
    if (access(path, F_OK) == 0) {
        fprintf(stderr,
                "error: output already exists: %s; pass --force to replace it\n",
                path);
        exit(EXIT_FAILURE);
    }
    if (errno != ENOENT) {
        fail_errno("checking output path");
    }
}

int main(int argc, char **argv)
{
    Options options = parse_options(argc, argv);
    check_output_before_search(options.output, options.force);
    install_signal_handlers();

    Result results[MAX_N - 2];
    int result_count = 0;
    for (int n = options.min_n; n <= options.max_n; ++n) {
        results[result_count++] = run_search(n, options.progress_every);
        if (stop_requested) {
            fprintf(stderr,
                    "interrupted during n=%d; no result file was written\n", n);
            return 130;
        }
    }
    write_results_atomic(options.output, results, result_count, options.force);

    fputs("flattened terms: [", stdout);
    bool first = true;
    for (int i = 0; i < result_count; ++i) {
        for (int k = 3; k <= results[i].n; ++k) {
            if (!first) {
                fputs(", ", stdout);
            }
            printf("%d", results[i].best[k]);
            first = false;
        }
    }
    printf("]\nwrote %s\n", options.output);
    return EXIT_SUCCESS;
}
