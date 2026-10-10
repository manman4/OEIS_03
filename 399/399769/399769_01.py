"""Compute A399769 for n = 1..10000 and write b39969.txt.

The length counts divisors, including 1. In particular, a(1) = 1.
All calculations use exact Python integer arithmetic.
"""

from pathlib import Path


LIMIT = 10000
OUTPUT = Path(__file__).resolve().with_name("b39969.txt")


def sequence(limit: int) -> list[int]:
    """Return a(1), ..., a(limit) using longest-path dynamic programming."""
    divisors: list[list[int]] = [[] for _ in range(limit + 1)]
    for d in range(1, limit + 1):
        for n in range(d, limit + 1, d):
            divisors[n].append(d)

    values = []
    for n in range(1, limit + 1):
        ds = divisors[n]  # Increasing order, so all predecessors are processed.
        lengths = [1] * len(ds)
        for j, d in enumerate(ds):
            for i in range(j):
                if (d - 1) % ds[i] == 0:
                    lengths[j] = max(lengths[j], lengths[i] + 1)
        values.append(max(lengths))
    return values


def main() -> None:
    values = sequence(LIMIT)
    OUTPUT.write_text(
        "".join(f"{n} {value}\n" for n, value in enumerate(values, 1)),
        encoding="ascii",
    )
    print(f"Saved {len(values)} terms to {OUTPUT.name}")


if __name__ == "__main__":
    main()
