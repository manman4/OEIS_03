#!/usr/bin/env python3
"""Naively compute anti-Ramsey numbers for paths in complete graphs.

For every n, the program enumerates every partition of E(K_n) into nonempty
color classes.  Color names are irrelevant, so restricted-growth strings give
exactly one representative of each coloring up to a permutation of the colors.
For each 3 <= k <= n, every undirected copy of P_k is then checked directly.

The number of set partitions is the Bell number B_(n choose 2).  In particular,
B_10 = 115975 for n=5 but B_15 = 1382958545 for n=6, so this deliberately
naive implementation refuses n > 5.
"""

from __future__ import annotations

import argparse
import json
import os
import tempfile
from collections.abc import Iterator, Sequence
from itertools import combinations, permutations
from pathlib import Path


HERE = Path(__file__).resolve().parent
DEFAULT_OUTPUT = HERE / "results_paths_bruteforce.json"
MAX_N = 5

Edge = tuple[int, int]
Coloring = tuple[int, ...]


def edge_pairs(n: int) -> list[Edge]:
    """Return the edges of K_n in lexicographic order."""
    return list(combinations(range(n), 2))


def canonical_colorings(edge_count: int) -> Iterator[Coloring]:
    """Yield all edge colorings up to a permutation of color names.

    A restricted-growth string starts with 0 and every later entry is at most
    one greater than the largest preceding entry.  These strings are in
    bijection with set partitions of the edge positions.
    """
    if edge_count < 1:
        raise ValueError("edge_count must be positive")

    colors = [0] * edge_count

    def visit(index: int, largest_color: int) -> Iterator[Coloring]:
        if index == edge_count:
            yield tuple(colors)
            return

        for color in range(largest_color + 2):
            colors[index] = color
            yield from visit(index + 1, max(largest_color, color))

    yield from visit(1, 0)


def bell_number(size: int) -> int:
    """Return the number of partitions of a set of the given size."""
    if size < 0:
        raise ValueError("size must be nonnegative")

    row = [1]
    for _ in range(size):
        next_row = [row[-1]]
        next_row.extend(next_row[-1] + value for value in row)
        row = next_row
    return row[0]


def path_edge_indices(n: int, k: int) -> tuple[tuple[int, ...], ...]:
    """Return all undirected labeled copies of P_k in K_n.

    A path and its reversal have the same edges.  Requiring the first vertex
    to be smaller than the last retains exactly one of the two orientations.
    """
    if not 2 <= k <= n:
        raise ValueError("require 2 <= k <= n")

    lookup = {edge: index for index, edge in enumerate(edge_pairs(n))}
    paths: list[tuple[int, ...]] = []
    for order in permutations(range(n), k):
        if order[0] > order[-1]:
            continue
        indices = []
        for u, v in zip(order, order[1:]):
            edge = (u, v) if u < v else (v, u)
            indices.append(lookup[edge])
        paths.append(tuple(indices))
    return tuple(paths)


def contains_rainbow_path(
    coloring: Sequence[int], paths: Sequence[Sequence[int]]
) -> bool:
    """Return whether a listed path has pairwise distinct edge colors."""
    return any(len({coloring[index] for index in path}) == len(path) for path in paths)


def color_classes(coloring: Coloring, edges: Sequence[Edge]) -> list[list[list[int]]]:
    """Decode a canonical coloring as color classes of endpoint pairs."""
    classes: list[list[list[int]]] = [[] for _ in range(max(coloring) + 1)]
    for edge, color in zip(edges, coloring):
        classes[color].append(list(edge))
    return classes


def enumerate_for_n(n: int, progress_every: int) -> dict[str, object]:
    """Compute AR(n, P_k) for every k in 3,...,n."""
    if not 3 <= n <= MAX_N:
        raise ValueError(f"the naive enumerator supports 3 <= n <= {MAX_N}")

    edges = edge_pairs(n)
    paths_by_k = {k: path_edge_indices(n, k) for k in range(3, n + 1)}
    best = {k: -1 for k in paths_by_k}
    witnesses: dict[int, Coloring] = {}
    partition_count = 0

    for coloring in canonical_colorings(len(edges)):
        partition_count += 1
        number_of_colors = max(coloring) + 1

        for k, paths in paths_by_k.items():
            if number_of_colors <= best[k]:
                continue
            if not contains_rainbow_path(coloring, paths):
                best[k] = number_of_colors
                witnesses[k] = coloring

        if progress_every and partition_count % progress_every == 0:
            current = ", ".join(f"AR(n,P_{k})={best[k]}" for k in sorted(best))
            print(f"n={n}: tested {partition_count} partitions; {current}")

    expected_count = bell_number(len(edges))
    if partition_count != expected_count:
        raise RuntimeError(
            f"partition count mismatch: got {partition_count}, expected {expected_count}"
        )
    if set(witnesses) != set(paths_by_k):
        raise RuntimeError("failed to find a witness for every path length")

    entries = [
        {
            "k": k,
            "anti_ramsey": best[k],
            "witness_color_classes": color_classes(witnesses[k], edges),
        }
        for k in sorted(best)
    ]
    return {
        "n": n,
        "edge_count": len(edges),
        "canonical_coloring_count": partition_count,
        "entries": entries,
    }


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--min-n", type=int, default=3)
    parser.add_argument("--max-n", type=int, default=MAX_N)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument(
        "--progress-every",
        type=int,
        default=10_000,
        help="print progress after this many partitions; use 0 to disable",
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="replace an existing output file",
    )
    return parser.parse_args()


def write_json_atomic(path: Path, payload: object, force: bool) -> None:
    """Write JSON atomically, refusing to replace output unless requested."""
    if path.exists() and not force:
        raise FileExistsError(f"output already exists: {path}; pass --force to replace it")

    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary_name = tempfile.mkstemp(
        prefix=f".{path.name}.", suffix=".tmp", dir=path.parent
    )
    temporary_path = Path(temporary_name)
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8") as stream:
            json.dump(payload, stream, indent=2)
            stream.write("\n")
        os.replace(temporary_path, path)
    except BaseException:
        temporary_path.unlink(missing_ok=True)
        raise


def main() -> None:
    args = parse_args()
    if not 3 <= args.min_n <= args.max_n <= MAX_N:
        raise SystemExit(f"require 3 <= min-n <= max-n <= {MAX_N}")
    if args.progress_every < 0:
        raise SystemExit("progress-every must be nonnegative")

    values = [
        enumerate_for_n(n, args.progress_every)
        for n in range(args.min_n, args.max_n + 1)
    ]
    flattened_terms = [
        entry["anti_ramsey"]
        for value in values
        for entry in value["entries"]
    ]
    payload = {
        "problem": 1105,
        "object": "AR(n, P_k) for 3 <= k <= n",
        "method": (
            "all set partitions of E(K_n), represented by restricted-growth "
            "strings; every labeled path tested directly"
        ),
        "values": values,
        "flattened_terms": flattened_terms,
    }
    write_json_atomic(args.output, payload, args.force)

    for value in values:
        row = ", ".join(
            f"AR({value['n']},P_{entry['k']})={entry['anti_ramsey']}"
            for entry in value["entries"]
        )
        print(
            f"n={value['n']}: colorings={value['canonical_coloring_count']}; {row}"
        )
    print(f"flattened terms: {flattened_terms}")
    print(f"wrote {args.output}")


if __name__ == "__main__":
    main()
