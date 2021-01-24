# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333795(n):
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    points = [i + 1 for i in range(n * n) if i % n - i // n == 0 or i % n + i // n == n - 1]
    for i in points:
        cycles = cycles.including(i)
    return cycles.len()

print([A333795(n) for n in range(2, 10)])