# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333903(n):
    universe = tl.grid(2 * n - 1, n - 1)
    GraphSet.set_universe(universe)
    start, goal = 1, n * (2 * n - 1) + 1
    paths = GraphSet.paths(start, goal, is_hamilton=True)
    return paths.len()

for n in range(1, 15, 2):
    print([n, A333903(n)]) 