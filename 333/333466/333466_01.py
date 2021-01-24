# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333466(n):
    if n == 1: return 1
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    for i in [1, n, n * (n - 1) + 1, n * n]:
        cycles = cycles.including(i)
    return cycles.len()

print([A333466(n) for n in range(1, 10)])
