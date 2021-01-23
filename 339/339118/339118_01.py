# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A(n, k):
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    return cycles.len()

def A339118(n):
    return A(6, n)

print([A339118(n) for n in range(2, 13)])
# print(A(11,6))
# print(A(13,6))