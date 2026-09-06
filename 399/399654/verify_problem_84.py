#!/usr/bin/env python3
"""Small checks for both Problem #84 implementations.

This filename deliberately does not begin with ``test_`` so the repository's
normal pytest run does not acquire an undeclared NetworkX dependency.
"""

from __future__ import annotations

import networkx as nx

from enumerate_labeled import decode_cycle_set, enumerate_cycle_sets
from enumerate_unlabeled import cycle_lengths_direct


def test_direct_cycle_detection() -> None:
    assert cycle_lengths_direct(nx.path_graph(6)) == ()
    assert cycle_lengths_direct(nx.cycle_graph(5)) == (5,)
    assert cycle_lengths_direct(nx.complete_graph(5)) == (3, 4, 5)


def test_small_exact_agreement() -> None:
    for n in range(3, 6):
        labeled = {
            tuple(decode_cycle_set(spectrum_mask, n))
            for spectrum_mask in enumerate_cycle_sets(n)
        }
        atlas = {
            cycle_lengths_direct(graph)
            for graph in nx.graph_atlas_g()
            if graph.number_of_nodes() == n
        }
        assert labeled == atlas, (
            f"n={n}: labeled-only={sorted(labeled - atlas)}, "
            f"atlas-only={sorted(atlas - labeled)}"
        )


def main() -> None:
    test_direct_cycle_detection()
    test_small_exact_agreement()
    print("ok - Problem #84 checks passed for n=3,4,5")


if __name__ == "__main__":
    main()
