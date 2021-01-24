# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333864(n):
    universe = tl.grid(2 * n - 1, n - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles(is_hamilton=True)
    return cycles.len()

for n in [9,11,13]:
    print(A333864(n))
