# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333759(n):
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    points = [i for i in range(1, n * n + 1) if i % n < 2 or ((i - 1) // n + 1) % n < 2]
    # print(points)
    for i in points:
        cycles = cycles.including(i)
    return cycles.len()

print([A333759(n) for n in range(2, 10)])