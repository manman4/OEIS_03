#!/usr/bin/env python3
"""Exact cycle-set enumeration using all non-isomorphic small graphs.

NetworkX's Graph Atlas supplies the graphs. Cycle lengths are detected directly
from vertex orderings, independently of enumerate_labeled.py's edge-mask subset
propagation.
"""

from __future__ import annotations

import argparse
import json
from itertools import combinations, permutations
from pathlib import Path

import networkx as nx


HERE = Path(__file__).resolve().parent
DEFAULT_OUTPUT = HERE / "results_unlabeled.json"


def contains_cycle_on_vertices(graph: nx.Graph, vertices: tuple[int, ...]) -> bool:
    """Return whether graph has a cycle whose vertex set is exactly vertices."""
    start = min(vertices)
    others = tuple(vertex for vertex in vertices if vertex != start)

    for tail in permutations(others):
        # The two orientations describe the same undirected cycle.
        if tail[0] > tail[-1]:
            continue
        order = (start, *tail)
        if all(
            graph.has_edge(order[index], order[(index + 1) % len(order)])
            for index in range(len(order))
        ):
            return True
    return False


def cycle_lengths_direct(graph: nx.Graph) -> tuple[int, ...]:
    """Find all simple-cycle lengths by checking vertex subsets and orderings."""
    nodes = tuple(sorted(graph.nodes()))
    lengths: list[int] = []

    for length in range(3, len(nodes) + 1):
        if any(
            contains_cycle_on_vertices(graph, vertices)
            for vertices in combinations(nodes, length)
        ):
            lengths.append(length)
    return tuple(lengths)


def witness_edges(graph: nx.Graph) -> list[list[int]]:
    return [[min(u, v), max(u, v)] for u, v in sorted(graph.edges())]


def atlas_graphs_on_n_vertices(n: int) -> list[nx.Graph]:
    """Return every non-isomorphic simple graph on n vertices from the atlas."""
    return [graph for graph in nx.graph_atlas_g() if graph.number_of_nodes() == n]


def result_for_n(n: int) -> dict[str, object]:
    if not 1 <= n <= 7:
        raise ValueError("the NetworkX Graph Atlas supports 1 <= n <= 7")

    graphs = atlas_graphs_on_n_vertices(n)
    witnesses: dict[tuple[int, ...], nx.Graph] = {}
    for graph in graphs:
        lengths = cycle_lengths_direct(graph)
        witnesses.setdefault(lengths, graph)

    cycle_sets = [
        {
            "lengths": list(lengths),
            "witness_edges": witness_edges(witnesses[lengths]),
        }
        for lengths in sorted(witnesses)
    ]
    return {
        "n": n,
        "unlabeled_graph_count": len(graphs),
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
        "method": "NetworkX Graph Atlas; direct vertex-order cycle checks",
        "networkx_version": nx.__version__,
        "values": [result_for_n(n) for n in range(args.min_n, args.max_n + 1)],
    }
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    for value in payload["values"]:
        print(f"n={value['n']}: f(n)={value['f_n']}")
    print(f"wrote {args.output}")


if __name__ == "__main__":
    main()
