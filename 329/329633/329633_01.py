# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A329633(n):
    if n == 1: return [1]
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    start, goal = 1, n
    paths = GraphSet.paths(start, goal)
    return [paths.len(n - 1 + 2 * k).len() for k in range(n * (n - 1) // 2 + 1)]

cnt = 1
for i in range(1, 10):
    for j in A329633(i):
        print(str(cnt) + ' ' + str(j))
        cnt += 1