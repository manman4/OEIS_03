# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A302337(n):
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    return [cycles.len(2 * k).len() for k in range(2, n * n // 2 + 1)]

# print([i for n in range(2, 8) for i in A302337(n)]) 
cnt = 2
for i in range(2, 10):
    for j in A302337(i):
        print(str(cnt) + ' ' + str(j))
        cnt += 1