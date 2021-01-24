# Using graphillion
from graphillion import GraphSet
import graphillion.tutorial as tl

def A333520(n):
    if n == 1: return [1]
    universe = tl.grid(n - 1, n - 1)
    GraphSet.set_universe(universe)
    start, goal = 1, n * n
    paths = GraphSet.paths(start, goal)
    return [paths.len(2 * (n - 1 + k)).len() for k in range((n - 1) ** 2 // 2 + 1)]

# print([i for n in range(1, 2) for i in A333520(n)])


#     return sum([paths.len(2 * (n - 1 + k)).len() for k in range((n - 1) ** 2 // 2 + 1)])

# for n in range(1, 15):
#     print(A333520(n))


# for n in range(1, 8):
#     print(A302337(n))


cnt = 1
for i in range(1, 10):
    for j in A333520(i):
        print(str(cnt) + ' ' + str(j))
        cnt += 1