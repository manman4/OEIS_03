# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A(n, k):
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    return cycles.len()

def A358707(n):
    return A(n, 10)

for n in range(2, 15):
    print(A358707(n))
