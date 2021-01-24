# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A271592(n, k):
    if k == 1: return 1
    universe = tl.grid(k - 1, n - 1)
    GraphSet.set_universe(universe)
    start, goal = 1, n
    paths = GraphSet.paths(start, goal, is_hamilton=True)
    return paths.len()

def A333606(n):
    return A271592(10, n)

print([A333606(n) for n in range(1, 9)])
