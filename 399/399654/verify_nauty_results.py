#!/usr/bin/env python3
"""Verify nauty output and compare its overlap with the two earlier methods."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import networkx as nx

from enumerate_nauty import cycle_lengths_dfs, decode_graph6_adjacency


HERE = Path(__file__).resolve().parent
NAUTY_RESULTS = HERE / "results_nauty.json"
EARLIER_RESULTS = (
    HERE / "results_labeled.json",
    HERE / "results_unlabeled.json",
)

# Counts of non-isomorphic simple graphs. These also detect truncated or
# accidentally filtered geng output.
KNOWN_GRAPH_COUNTS = {
    1: 1,
    2: 2,
    3: 4,
    4: 11,
    5: 34,
    6: 156,
    7: 1_044,
    8: 12_346,
    9: 274_668,
    10: 12_005_168,
    11: 1_018_997_864,
}

# Exact values reported through n=10 in Alvin Dunås's 2026 Uppsala thesis.
KNOWN_CYCLE_SET_COUNTS = {
    1: 1,
    2: 1,
    3: 2,
    4: 4,
    5: 6,
    6: 11,
    7: 21,
    8: 40,
    9: 75,
    10: 133,
}


def cycle_sets_by_n(path: Path) -> dict[int, set[tuple[int, ...]]]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    return {
        int(value["n"]): {
            tuple(int(length) for length in item["lengths"])
            for item in value["cycle_sets"]
        }
        for value in payload["values"]
    }


def verify_nauty_file(path: Path) -> dict[int, set[tuple[int, ...]]]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    result: dict[int, set[tuple[int, ...]]] = {}

    for value in payload["values"]:
        n = int(value["n"])
        graph_count = int(value["unlabeled_graph_count"])
        assert graph_count == KNOWN_GRAPH_COUNTS[n], (
            f"n={n}: got {graph_count} graphs, expected {KNOWN_GRAPH_COUNTS[n]}"
        )

        observed: set[tuple[int, ...]] = set()
        for item in value["cycle_sets"]:
            claimed = tuple(int(length) for length in item["lengths"])
            graph6 = item["witness_graph6"].encode("ascii")
            adjacency = decode_graph6_adjacency(graph6)
            networkx_graph = nx.from_graph6_bytes(graph6)
            networkx_adjacency = tuple(
                sum(1 << neighbor for neighbor in networkx_graph.neighbors(vertex))
                for vertex in range(n)
            )
            assert adjacency == networkx_adjacency, (
                f"n={n}: local and NetworkX graph6 decoders disagree"
            )
            actual = cycle_lengths_dfs(adjacency)
            assert len(adjacency) == n
            assert actual == claimed, (
                f"n={n}: witness claims {claimed}, detector gives {actual}"
            )
            observed.add(claimed)

        assert len(observed) == int(value["f_n"])
        expected_cycle_set_count = KNOWN_CYCLE_SET_COUNTS.get(n)
        if expected_cycle_set_count is not None:
            assert len(observed) == expected_cycle_set_count, (
                f"n={n}: got f(n)={len(observed)}, "
                f"expected {expected_cycle_set_count}"
            )
        result[n] = observed
        reference_note = (
            "" if expected_cycle_set_count is not None else " (no reference f(n))"
        )
        print(
            f"verified nauty n={n}: graphs={graph_count}, "
            f"f(n)={len(observed)}{reference_note}"
        )

    return result


def compare_earlier_results(
    nauty: dict[int, set[tuple[int, ...]]], nauty_results: Path
) -> None:
    for path in EARLIER_RESULTS:
        earlier = cycle_sets_by_n(path)
        overlap = sorted(set(nauty) & set(earlier))
        if not overlap:
            print(f"no overlapping n values with {path.name}; comparison skipped")
            continue
        for n in overlap:
            assert nauty[n] == earlier[n], (
                f"n={n}: nauty and {path.name} disagree on complete cycle-set lists"
            )
        print(f"matched {path.name} exactly for n={overlap}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--input",
        type=Path,
        default=NAUTY_RESULTS,
        help=f"nauty result JSON (default: {NAUTY_RESULTS})",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    nauty = verify_nauty_file(args.input)
    compare_earlier_results(nauty, args.input)
    print("ok - nauty results and all available cross-checks passed")


if __name__ == "__main__":
    main()
