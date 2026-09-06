#!/usr/bin/env python3
"""Exact cycle-set enumeration using all labeled graphs.

The implementation represents both graphs and cycles as bit masks over the
edges of K_n. A subset zeta propagation collects, for every graph, all cycle
edge masks contained in it.
"""

from __future__ import annotations

import argparse
import json
from itertools import combinations, permutations
from pathlib import Path
from typing import Iterable


HERE = Path(__file__).resolve().parent
DEFAULT_OUTPUT = HERE / "results_labeled.json"


def edge_pairs(n: int) -> list[tuple[int, int]]:
    """Return the edges of K_n in the fixed order used by every bit mask."""
    return list(combinations(range(n), 2))


def edge_lookup(n: int) -> dict[tuple[int, int], int]:
    return {edge: index for index, edge in enumerate(edge_pairs(n))}


def edge_mask(
    edges: Iterable[tuple[int, int]], lookup: dict[tuple[int, int], int]
) -> int:
    mask = 0
    for u, v in edges:
        edge = (u, v) if u < v else (v, u)
        mask |= 1 << lookup[edge]
    return mask


def all_cycle_masks(n: int) -> dict[int, tuple[int, ...]]:
    """Return every distinct labeled cycle edge mask, grouped by length.

    The smallest vertex is fixed as the first vertex, removing rotations. Of
    the two orientations, only the one whose second vertex is smaller than its
    final vertex is retained.
    """
    lookup = edge_lookup(n)
    result: dict[int, tuple[int, ...]] = {}

    for length in range(3, n + 1):
        masks: set[int] = set()
        for vertices in combinations(range(n), length):
            start = vertices[0]
            for tail in permutations(vertices[1:]):
                if tail[0] > tail[-1]:
                    continue
                order = (start, *tail)
                cycle_edges = [
                    (order[index], order[(index + 1) % length])
                    for index in range(length)
                ]
                masks.add(edge_mask(cycle_edges, lookup))
        result[length] = tuple(sorted(masks))

    return result


def decode_cycle_set(spectrum_mask: int, n: int) -> list[int]:
    return [
        length
        for length in range(3, n + 1)
        if spectrum_mask & (1 << (length - 3))
    ]


def decode_edges(graph_mask: int, n: int) -> list[list[int]]:
    return [
        [u, v]
        for index, (u, v) in enumerate(edge_pairs(n))
        if graph_mask & (1 << index)
    ]


def enumerate_cycle_sets(n: int) -> dict[int, int]:
    """Map each realized spectrum mask to one labeled witness graph mask."""
    if not 1 <= n <= 7:
        raise ValueError("the labeled enumerator supports 1 <= n <= 7")

    edge_count = n * (n - 1) // 2
    graph_count = 1 << edge_count

    # A byte is sufficient because n <= 7 gives at most five possible lengths.
    spectra = bytearray(graph_count)
    for length, masks in all_cycle_masks(n).items():
        length_bit = 1 << (length - 3)
        for mask in masks:
            spectra[mask] |= length_bit

    # Boolean-lattice zeta propagation. After processing every edge bit,
    # spectra[G] is the union of cycle lengths from every cycle mask C subset G.
    for edge_index in range(edge_count):
        edge_bit = 1 << edge_index
        block_size = edge_bit << 1
        for block_start in range(0, graph_count, block_size):
            upper_start = block_start + edge_bit
            for offset in range(edge_bit):
                spectra[upper_start + offset] |= spectra[block_start + offset]

    witnesses: dict[int, int] = {}
    for graph_mask, spectrum_mask in enumerate(spectra):
        witnesses.setdefault(spectrum_mask, graph_mask)
    return witnesses


def result_for_n(n: int) -> dict[str, object]:
    witnesses = enumerate_cycle_sets(n)
    cycle_sets = [
        {
            "lengths": decode_cycle_set(spectrum_mask, n),
            "witness_edges": decode_edges(witnesses[spectrum_mask], n),
        }
        for spectrum_mask in sorted(witnesses)
    ]
    return {
        "n": n,
        "labeled_graph_count": 1 << (n * (n - 1) // 2),
        "f_n": len(cycle_sets),
        "cycle_sets": cycle_sets,
    }


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--min-n", type=int, default=3)
    parser.add_argument("--max-n", type=int, default=6)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if not 1 <= args.min_n <= args.max_n <= 7:
        raise SystemExit("require 1 <= min-n <= max-n <= 7")

    payload = {
        "method": "all labeled graphs; cycle-mask subset propagation",
        "values": [result_for_n(n) for n in range(args.min_n, args.max_n + 1)],
    }
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    for value in payload["values"]:
        print(f"n={value['n']}: f(n)={value['f_n']}")
    print(f"wrote {args.output}")


if __name__ == "__main__":
    main()
