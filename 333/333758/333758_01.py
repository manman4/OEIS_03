# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333758(n, k):
    universe = tl.grid(n - 1, k - 1)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    points = [i for i in range(1, k * n + 1) if i % k < 2 or ((i - 1) // k + 1) % n < 2]
    for i in points:
        cycles = cycles.including(i)
    return cycles.len()

print([A333758(j + 2, i - j + 2) for i in range(11 - 1) for j in range(i + 1)])

cnt = 2
for i in range(14 - 1):
    for j in range(i + 1): 
        print(str(cnt) + ' ' + str(A333758(j + 2, i - j + 2)))
        cnt += 1


