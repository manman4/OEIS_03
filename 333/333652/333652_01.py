# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333652(n):
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles().including(1).including(n)
    return [cycles.len(2 * k).len() for k in range(n, n * n // 2 + 1)]

cnt = 2
for i in range(2, 10):
    for j in A333652(i):
        print(str(cnt) + ' ' + str(j))
        cnt += 1