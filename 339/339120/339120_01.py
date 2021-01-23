# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A(n, k):
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    return cycles.len()

def A339120(n):
    return A(n, 8)

print([A339120(n) for n in range(2, 8)])
