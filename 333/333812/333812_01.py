# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A064298(n, k):
    if n == 1 or k == 1: return 1
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    start, goal = 1, k * n
    paths = GraphSet.paths(start, goal)
    return paths.len()

def A333812(n):
    return A064298(n, 7)

cnt = 1
for i in range(1, 401):
    print(str(cnt) + ' ' + str(A333812(i)))
    cnt += 1


