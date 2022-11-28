# Using graphillion

from graphillion import GraphSet
import graphillion.tutorial as tl

def A333513(n, k):
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    for i in [1, k, k * (n - 1) + 1, k * n]:
        cycles = cycles.including(i)
    return cycles.len()

def A358713(n):
    return A333513(n, 7)

for i in range(2, 51):
    print(str(i) + ' ' + str(A358713(i)))

# print([A358713(i) for i in range(2, 22)])