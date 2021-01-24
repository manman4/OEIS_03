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

def A333605(n):
    return A271592(9, 2 * n + 1)

for i in range(201):
    print(str(i) + ' ' + str(A333605(i)))
