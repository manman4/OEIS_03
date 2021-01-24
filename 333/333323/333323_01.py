# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333323(n):
    if n == 1: return 1
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles().including(1).including(n * n)
    return cycles.len()

print([A333323(n) for n in range(1, 10)])
