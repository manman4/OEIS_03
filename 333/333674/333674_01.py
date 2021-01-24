# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333674(n):
    universe = tl.grid(2 * n, 2 * n)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles().including(2 * n * n + 2 * n + 1)
    return cycles.len()

print([A333674(n) for n in range(1, 7)])