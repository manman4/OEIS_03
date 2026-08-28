/*
 * A332800 -- one-word packed-cache variant of 332800_04.c.
 *
 * The exact DFS recurrence and its high-remainder shortcut are shared with
 * 04.  Version 05 changes the bounded memoization algorithm: a 43-bit state
 * key and its exact count are published together in one atomic uint64_t.
 * The remaining 21 bits hold counts through 2^21-1.  A larger subtree count
 * is never truncated; it is simply not cached and is recomputed if needed.
 * Therefore cache admission and replacement affect running time only.
 *
 * Each two-way bucket is 16 bytes instead of 04's 24 bytes.  With the same
 * --memory-mb setting this gives 1.5 times as many buckets and entries, while
 * a reader needs one atomic load rather than the key/count/key validation of
 * 04.  The whole key-count pair is a single atomic object, so concurrent
 * replacement cannot combine a key with another state's count.
 *
 * Eight workers are the default because that matches the tested machine's
 * physical execution capacity and was faster than oversubscribing it with
 * 16 workers.  --threads can still override this without changing results or
 * the cache allocation.
 *
 * Build:
 *   clang -O3 -std=c11 -Wall -Wextra -Wpedantic -pthread \
 *       332800_05.c -o 332800_05
 *
 * Usage examples:
 *   ./332800_05 --upto 32 --memory-mb 1024
 *   ./332800_05 --term 32 --memory-mb 1024 --threads 8 --verbose
 *   ./332800_05 --check
 *
 * Completed terms are flushed immediately.  --upto atomically checkpoints
 * b332800_05.txt after every completed term; --term and --check do not modify
 * the b-file.
 */

#define PACKED_CACHE 1
#define PROGRAM_LABEL "332800_05"
#define DEFAULT_THREADS 8
#define BFILE_NAME "b332800_05.txt"
#define BFILE_TEMP_TEMPLATE BFILE_NAME ".tmp.XXXXXX"
#define BFILE_LOCK_NAME BFILE_NAME ".lock"

#include "332800_04.c"
