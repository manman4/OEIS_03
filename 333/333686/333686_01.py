# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333685(n, k):
    if n == 0 or k == 0: return 1
    universe = tl.grid(2 * n, 2 * k)
    GraphSet.set_universe(universe)
    start, goal = 1, (2 * n + 1) * (2 * k + 1)
    paths = GraphSet.paths(start, goal).including((start + goal) // 2)
    return paths.len()

def A333686(n):
    return A333685(n, 1)

cnt = 0
for i in range(501):
    print(str(cnt) + ' ' + str(A333686(i)))
    cnt += 1
