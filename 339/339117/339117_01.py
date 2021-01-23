# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A(n, k):
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    return cycles.len()

def A339117(n):
    return A(n, 5)

# print([A339117(n) for n in range(2, 15)])
for n in range(2, 501):
    print(str(n) + " " + str(A339117(n)))

