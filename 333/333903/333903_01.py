# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333903(n):
    universe = tl.grid(n - 1, 2 * n - 1)
    GraphSet.set_universe(universe)
    start, goal = 1, 2 * n
    paths = GraphSet.paths(start, goal, is_hamilton=True)
    return paths.len()

print([A333903(n) for n in range(1, 8)])