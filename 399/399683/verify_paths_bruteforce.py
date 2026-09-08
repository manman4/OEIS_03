#!/usr/bin/env python3
"""Independently verify saved path anti-Ramsey results."""

from __future__ import annotations

import argparse
import json
from itertools import combinations
from math import comb
from pathlib import Path
from typing import Any


HERE = Path(__file__).resolve().parent
DEFAULT_INPUT = HERE / "results_paths_bruteforce.json"

Edge = tuple[int, int]


def bell_number_from_stirling(size: int) -> int:
    """Compute B_size as the sum of Stirling numbers S(size, k)."""
    stirling = [0] * (size + 1)
    stirling[0] = 1
    for item_count in range(1, size + 1):
        next_stirling = [0] * (size + 1)
        for block_count in range(1, item_count + 1):
            next_stirling[block_count] = (
                stirling[block_count - 1]
                + block_count * stirling[block_count]
            )
        stirling = next_stirling
    return sum(stirling)


def published_value(n: int, k: int) -> int:
    """Return the known value of AR(n, P_k) for n >= k >= 3."""
    if not 3 <= k <= n:
        raise ValueError("require 3 <= k <= n")
    if k == 3:
        return 1
    if k == 4:
        return 3 if n == 4 else 2

    ell = (k - 1) // 2
    epsilon = 1 if k % 2 else 2
    return max(
        comb(k - 2, 2) + 1,
        comb(ell - 1, 2) + (ell - 1) * (n - ell + 1) + epsilon,
    )


def decode_witness(n: int, entry: dict[str, Any]) -> dict[Edge, int]:
    """Validate and decode the witness color classes."""
    expected_edges = set(combinations(range(n), 2))
    edge_colors: dict[Edge, int] = {}
    classes = entry.get("witness_color_classes")
    if not isinstance(classes, list) or not classes:
        raise ValueError("witness_color_classes must be a nonempty list")

    for color, color_class in enumerate(classes):
        if not isinstance(color_class, list) or not color_class:
            raise ValueError(f"color class {color} is empty or invalid")
        for raw_edge in color_class:
            if (
                not isinstance(raw_edge, list)
                or len(raw_edge) != 2
                or not all(isinstance(vertex, int) for vertex in raw_edge)
            ):
                raise ValueError(f"invalid edge: {raw_edge!r}")
            u, v = raw_edge
            if not 0 <= u < v < n:
                raise ValueError(f"edge is not in canonical K_{n} form: {raw_edge!r}")
            edge = (u, v)
            if edge in edge_colors:
                raise ValueError(f"edge occurs in multiple color classes: {edge}")
            edge_colors[edge] = color

    if set(edge_colors) != expected_edges:
        missing = sorted(expected_edges - set(edge_colors))
        extra = sorted(set(edge_colors) - expected_edges)
        raise ValueError(f"witness is not a partition of E(K_{n}); {missing=}, {extra=}")
    if len(classes) != entry.get("anti_ramsey"):
        raise ValueError("number of witness classes does not equal anti_ramsey")
    return edge_colors


def has_rainbow_path(n: int, k: int, edge_colors: dict[Edge, int]) -> bool:
    """Find a rainbow P_k by a vertex-by-vertex depth-first search."""

    def search(last: int, used_vertices: int, used_colors: int, depth: int) -> bool:
        if depth == k:
            return True
        for vertex in range(n):
            vertex_bit = 1 << vertex
            if used_vertices & vertex_bit:
                continue
            edge = (last, vertex) if last < vertex else (vertex, last)
            color_bit = 1 << edge_colors[edge]
            if used_colors & color_bit:
                continue
            if search(
                vertex,
                used_vertices | vertex_bit,
                used_colors | color_bit,
                depth + 1,
            ):
                return True
        return False

    return any(search(start, 1 << start, 0, 1) for start in range(n))


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    payload = json.loads(args.input.read_text(encoding="utf-8"))
    if payload.get("problem") != 1105:
        raise SystemExit("input does not identify Erdős problem 1105")

    values = payload.get("values")
    if not isinstance(values, list) or not values:
        raise SystemExit("values must be a nonempty list")

    recomputed_flattened_terms: list[int] = []
    previous_n: int | None = None
    for value in values:
        n = value.get("n")
        if not isinstance(n, int) or n < 3:
            raise SystemExit("every n must be an integer at least 3")
        if previous_n is not None and n != previous_n + 1:
            raise SystemExit("n values must be consecutive")
        previous_n = n

        edge_count = comb(n, 2)
        if value.get("edge_count") != edge_count:
            raise SystemExit(f"n={n}: incorrect edge_count")
        expected_colorings = bell_number_from_stirling(edge_count)
        if "canonical_coloring_count" in value:
            if value["canonical_coloring_count"] != expected_colorings:
                raise SystemExit(f"n={n}: incorrect canonical coloring count")
            count_description = (
                f"{expected_colorings} canonical colorings "
                f"(Bell number B_{edge_count})"
            )
        else:
            statistic_names = (
                "search_nodes",
                "terminal_colorings",
                "pruned_subtrees",
            )
            for name in statistic_names:
                statistic = value.get(name)
                if not isinstance(statistic, int) or statistic < 0:
                    raise SystemExit(f"n={n}: invalid or missing {name}")
            count_description = (
                f"pruned search statistics: nodes={value['search_nodes']}, "
                f"terminal={value['terminal_colorings']}, "
                f"pruned={value['pruned_subtrees']}"
            )

        entries = value.get("entries")
        if not isinstance(entries, list):
            raise SystemExit(f"n={n}: entries must be a list")
        if [entry.get("k") for entry in entries] != list(range(3, n + 1)):
            raise SystemExit(f"n={n}: k values are missing or out of order")

        for entry in entries:
            k = entry["k"]
            actual = entry.get("anti_ramsey")
            expected = published_value(n, k)
            if actual != expected:
                raise SystemExit(
                    f"n={n}, k={k}: got AR={actual}, published value is {expected}"
                )

            edge_colors = decode_witness(n, entry)
            if has_rainbow_path(n, k, edge_colors):
                raise SystemExit(f"n={n}, k={k}: witness contains a rainbow P_{k}")

            recomputed_flattened_terms.append(actual)
            print(
                f"verified n={n}, k={k}: AR(n,P_{k})={actual}, "
                f"witness colors={len(entry['witness_color_classes'])}"
            )

        print(f"verified n={n}: {count_description}")

    if recomputed_flattened_terms != payload.get("flattened_terms"):
        raise SystemExit("flattened_terms does not agree with the row data")
    print(
        "ok - search metadata, published values, witnesses, "
        "and flattened terms agree"
    )


if __name__ == "__main__":
    main()
