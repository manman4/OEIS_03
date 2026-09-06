#!/usr/bin/env python3
"""Compare the complete outputs of the two Problem #84 enumerators."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


HERE = Path(__file__).resolve().parent


def load_results(path: Path) -> dict[int, set[tuple[int, ...]]]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    result: dict[int, set[tuple[int, ...]]] = {}

    for value in payload["values"]:
        n = int(value["n"])
        cycle_sets = {
            tuple(int(length) for length in item["lengths"])
            for item in value["cycle_sets"]
        }
        if int(value["f_n"]) != len(cycle_sets):
            raise ValueError(f"{path}: inconsistent f_n for n={n}")
        if n in result:
            raise ValueError(f"{path}: duplicate n={n}")
        result[n] = cycle_sets
    return result


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--labeled", type=Path, default=HERE / "results_labeled.json"
    )
    parser.add_argument(
        "--unlabeled", type=Path, default=HERE / "results_unlabeled.json"
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    labeled = load_results(args.labeled)
    unlabeled = load_results(args.unlabeled)

    if set(labeled) != set(unlabeled):
        raise SystemExit(
            "different n ranges: "
            f"labeled={sorted(labeled)}, unlabeled={sorted(unlabeled)}"
        )

    failed = False
    for n in sorted(labeled):
        only_labeled = sorted(labeled[n] - unlabeled[n])
        only_unlabeled = sorted(unlabeled[n] - labeled[n])
        if only_labeled or only_unlabeled:
            failed = True
            print(f"MISMATCH n={n}")
            print(f"  only labeled:   {only_labeled}")
            print(f"  only unlabeled: {only_unlabeled}")
        else:
            print(f"verified n={n}: f(n)={len(labeled[n])}")

    if failed:
        raise SystemExit(1)
    print("all cycle sets agree exactly")


if __name__ == "__main__":
    main()
