# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333863(n):
    universe = tl.grid(4 * n + 1, 2 * n)
    GraphSet.set_universe(universe)
    start, goal = 1, 2 * (2 * n + 1) ** 2
    paths = GraphSet.paths(start, goal, is_hamilton=True)
    return paths.len()

print([A333863(n) for n in range(7)])
