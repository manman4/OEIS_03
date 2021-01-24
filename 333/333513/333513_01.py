# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333513(n, k):
    if n > k: return A333513(k, n)
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    for i in [1, k, k * (n - 1) + 1, k * n]:
        cycles = cycles.including(i)
    return cycles.len()

cnt = 2
for i in range(15 - 1):
     for j in range(i + 1):
         print(str(cnt) + ' ' + str(A333513(j + 2, i - j + 2)))
         cnt += 1