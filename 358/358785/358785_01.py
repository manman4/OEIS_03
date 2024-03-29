# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A(n, k):
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    return cycles.len()

def A358785(n):
    return A(n, 11)

for i in range(2, 41):
    print(str(i) + ' ' + str(A358785(i)))
