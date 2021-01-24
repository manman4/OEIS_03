# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333667(n):
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles().including(1).including(n * n)
    return [cycles.len(2 * k).len() for k in range(2 * n - 2, n * n // 2 + 1)]

cnt = 2
for i in range(2, 10):
    for j in A333667(i):
        print(str(cnt) + ' ' + str(j))
        cnt += 1