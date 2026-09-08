#!/usr/bin/env python3
"""Independently verify saved cycle anti-Ramsey results."""

from __future__ import annotations

import argparse
import json
from itertools import combinations
from math import comb, factorial
from pathlib import Path
from typing import Any


HERE = Path(__file__).resolve().parent
DEFAULT_INPUT = HERE / "results_cycles_bruteforce.json"

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
    """Return the known value of AR(n, C_k) for n >= k >= 3."""
    if not 3 <= k <= n:
        raise ValueError("require 3 <= k <= n")

    quotient, remainder = divmod(n, k - 1)
    number_of_blocks = quotient + (remainder > 0)
    return (
        quotient * comb(k - 1, 2)
        + comb(remainder, 2)
        + number_of_blocks
        - 1
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


def has_rainbow_cycle(n: int, k: int, edge_colors: dict[Edge, int]) -> bool:
    """Find a rainbow C_k by a vertex-by-vertex depth-first search."""

    def search(
        start: int,
        last: int,
        used_vertices: int,
        used_colors: int,
        depth: int,
    ) -> bool:
        if depth == k:
            closing_edge = (
                (last, start) if last < start else (start, last)
            )
            closing_color = 1 << edge_colors[closing_edge]
            return not used_colors & closing_color

        for vertex in range(n):
            vertex_bit = 1 << vertex
            if used_vertices & vertex_bit:
                continue
            edge = (last, vertex) if last < vertex else (vertex, last)
            color_bit = 1 << edge_colors[edge]
            if used_colors & color_bit:
                continue
            if search(
                start,
                vertex,
                used_vertices | vertex_bit,
                used_colors | color_bit,
                depth + 1,
            ):
                return True
        return False

    return any(search(start, start, 1 << start, 0, 1) for start in range(n))


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT)
    parser.add_argument(
        "--reference",
        type=Path,
        help="compare overlapping (n,k) values with another result file",
    )
    return parser.parse_args()


def result_map(payload: dict[str, Any]) -> dict[tuple[int, int], int]:
    """Return the saved anti-Ramsey values keyed by (n, k)."""
    values = payload.get("values")
    if not isinstance(values, list):
        raise ValueError("reference values must be a list")

    result: dict[tuple[int, int], int] = {}
    for value in values:
        n = value.get("n")
        entries = value.get("entries")
        if not isinstance(n, int) or not isinstance(entries, list):
            raise ValueError("invalid reference row")
        for entry in entries:
            k = entry.get("k")
            anti_ramsey = entry.get("anti_ramsey")
            if not isinstance(k, int) or not isinstance(anti_ramsey, int):
                raise ValueError("invalid reference entry")
            key = (n, k)
            if key in result:
                raise ValueError(f"duplicate reference entry: {key}")
            result[key] = anti_ramsey
    return result


def main() -> None:
    args = parse_args()
    payload = json.loads(args.input.read_text(encoding="utf-8"))
    if payload.get("problem") != 1105:
        raise SystemExit("input does not identify Erdős problem 1105")
    if payload.get("object") != "AR(n, C_k) for 3 <= k <= n":
        raise SystemExit("input does not identify the cycle anti-Ramsey triangle")

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
            expected_cycles = comb(n, k) * factorial(k - 1) // 2
            if entry.get("cycle_count") != expected_cycles:
                raise SystemExit(f"n={n}, k={k}: incorrect cycle_count")

            actual = entry.get("anti_ramsey")
            expected = published_value(n, k)
            if actual != expected:
                raise SystemExit(
                    f"n={n}, k={k}: got AR={actual}, published value is {expected}"
                )

            edge_colors = decode_witness(n, entry)
            if has_rainbow_cycle(n, k, edge_colors):
                raise SystemExit(f"n={n}, k={k}: witness contains a rainbow C_{k}")

            recomputed_flattened_terms.append(actual)
            print(
                f"verified n={n}, k={k}: AR(n,C_{k})={actual}, "
                f"cycles={expected_cycles}, "
                f"witness colors={len(entry['witness_color_classes'])}"
            )

        print(f"verified n={n}: {count_description}")

    if recomputed_flattened_terms != payload.get("flattened_terms"):
        raise SystemExit("flattened_terms does not agree with the row data")

    if args.reference is not None:
        reference_payload = json.loads(args.reference.read_text(encoding="utf-8"))
        if reference_payload.get("object") != payload.get("object"):
            raise SystemExit("reference describes a different object")
        current = result_map(payload)
        reference = result_map(reference_payload)
        overlap = sorted(set(current) & set(reference))
        if not overlap:
            raise SystemExit("no overlapping (n,k) values with reference")
        for key in overlap:
            if current[key] != reference[key]:
                raise SystemExit(
                    f"reference mismatch at n={key[0]}, k={key[1]}: "
                    f"got {current[key]}, reference has {reference[key]}"
                )
        print(f"matched reference for {len(overlap)} overlapping (n,k) values")

    print(
        "ok - counts, published values, witnesses, and flattened terms agree"
    )


if __name__ == "__main__":
    main()
