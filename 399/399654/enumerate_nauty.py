#!/usr/bin/env python3
"""Enumerate cycle sets by streaming non-isomorphic graphs from nauty geng.

This extends the small NetworkX Graph Atlas computation beyond seven vertices.
The graph6 decoder and cycle detector are implemented locally so that graphs do
not need to be materialized as NetworkX objects.
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
from pathlib import Path
from typing import BinaryIO


HERE = Path(__file__).resolve().parent
DEFAULT_OUTPUT = HERE / "results_nauty.json"


def find_geng(requested: str | None = None) -> str:
    """Locate geng under the common Homebrew and Debian executable names."""
    candidates = (requested,) if requested else ("geng", "nauty-geng")
    for candidate in candidates:
        if candidate:
            path = shutil.which(candidate)
            if path:
                return path
    names = ", ".join(name for name in candidates if name)
    raise FileNotFoundError(
        f"could not find {names}; install nauty or pass --geng /path/to/geng"
    )


def decode_graph6_adjacency(line: bytes) -> tuple[int, ...]:
    """Decode a graph6 record with at most 62 vertices into adjacency bitsets."""
    data = line.strip()
    prefix = b">>graph6<<"
    if data.startswith(prefix):
        data = data[len(prefix) :]
    if not data:
        raise ValueError("empty graph6 record")

    n = data[0] - 63
    if not 0 <= n <= 62:
        raise ValueError("only the one-byte graph6 order format (n <= 62) is supported")

    edge_count = n * (n - 1) // 2
    payload_size = (edge_count + 5) // 6
    payload = data[1:]
    if len(payload) != payload_size:
        raise ValueError(
            f"invalid graph6 payload for n={n}: expected {payload_size} bytes, "
            f"got {len(payload)}"
        )
    if any(not 63 <= byte <= 126 for byte in payload):
        raise ValueError("graph6 payload contains a byte outside 63..126")

    adjacency = [0] * n
    pair_index = 0
    for high in range(1, n):
        for low in range(high):
            byte = payload[pair_index // 6] - 63
            shift = 5 - (pair_index % 6)
            if byte & (1 << shift):
                adjacency[low] |= 1 << high
                adjacency[high] |= 1 << low
            pair_index += 1
    return tuple(adjacency)


def cycle_lengths_dfs(adjacency: tuple[int, ...]) -> tuple[int, ...]:
    """Return all simple-cycle lengths using minimum-rooted path searches.

    For each cycle, its smallest vertex is used as the root. Restricting all
    other path vertices to be larger than the root is complete and avoids
    rediscovering the same cycle from every possible starting vertex.
    """
    n = len(adjacency)
    if n < 3:
        return ()

    found_mask = 0
    all_lengths_mask = (1 << (n - 2)) - 1

    for start in range(n):
        start_bit = 1 << start
        allowed = ((1 << n) - 1) ^ ((1 << (start + 1)) - 1)
        first_vertices = adjacency[start] & allowed

        while first_vertices:
            first_bit = first_vertices & -first_vertices
            first_vertices ^= first_bit
            first = first_bit.bit_length() - 1
            stack = [(first, start_bit | first_bit, 2)]

            while stack:
                last, visited, path_length = stack.pop()

                if path_length >= 3 and adjacency[last] & start_bit:
                    found_mask |= 1 << (path_length - 3)
                    if found_mask == all_lengths_mask:
                        return tuple(range(3, n + 1))

                candidates = adjacency[last] & allowed & ~visited
                while candidates:
                    next_bit = candidates & -candidates
                    candidates ^= next_bit
                    next_vertex = next_bit.bit_length() - 1
                    stack.append(
                        (next_vertex, visited | next_bit, path_length + 1)
                    )

    return tuple(
        length
        for length in range(3, n + 1)
        if found_mask & (1 << (length - 3))
    )


def geng_records(executable: str, n: int) -> tuple[subprocess.Popen[bytes], BinaryIO]:
    """Start geng for exactly n vertices and return its process and stdout."""
    process = subprocess.Popen(
        [executable, "-q", str(n)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if process.stdout is None:
        process.kill()
        raise RuntimeError("failed to open geng stdout")
    return process, process.stdout


def result_for_n(executable: str, n: int) -> dict[str, object]:
    process, records = geng_records(executable, n)
    witnesses: dict[tuple[int, ...], str] = {}
    graph_count = 0

    try:
        for record in records:
            raw = record.strip()
            if not raw:
                continue
            adjacency = decode_graph6_adjacency(raw)
            if len(adjacency) != n:
                raise ValueError(
                    f"geng returned a graph on {len(adjacency)} vertices for n={n}"
                )
            lengths = cycle_lengths_dfs(adjacency)
            witnesses.setdefault(lengths, raw.decode("ascii"))
            graph_count += 1
    except BaseException:
        process.kill()
        process.wait()
        raise
    finally:
        records.close()

    stderr = process.stderr.read().decode("utf-8", errors="replace") if process.stderr else ""
    return_code = process.wait()
    if return_code != 0:
        raise RuntimeError(
            f"geng failed for n={n} with exit code {return_code}: {stderr.strip()}"
        )

    cycle_sets = [
        {"lengths": list(lengths), "witness_graph6": witnesses[lengths]}
        for lengths in sorted(witnesses)
    ]
    return {
        "n": n,
        "unlabeled_graph_count": graph_count,
        "f_n": len(cycle_sets),
        "cycle_sets": cycle_sets,
    }


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--min-n", type=int, default=3)
    parser.add_argument("--max-n", type=int, default=8)
    parser.add_argument("--geng", help="geng executable name or path")
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if not 1 <= args.min_n <= args.max_n <= 10:
        raise SystemExit("require 1 <= min-n <= max-n <= 10")

    executable = find_geng(args.geng)
    payload = {
        "method": "nauty geng; local graph6 decoder; minimum-rooted DFS",
        "geng_executable": executable,
        "values": [
            result_for_n(executable, n)
            for n in range(args.min_n, args.max_n + 1)
        ],
    }
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    for value in payload["values"]:
        print(
            f"n={value['n']}: graphs={value['unlabeled_graph_count']}, "
            f"f(n)={value['f_n']}"
        )
    print(f"wrote {args.output}")


if __name__ == "__main__":
    main()
