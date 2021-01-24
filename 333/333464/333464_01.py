# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333464(n):
    if n == 1: return 1
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    start, goal = 1, n * n
    paths = GraphSet.paths(start, goal)
    for i in range(n):
        paths = paths.including((n - 1) * (i + 1) + 1)
    return paths.len()

print([A333464(n) for n in range(1, 10)])
