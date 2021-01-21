# Using graphillion
from graphillion import GraphSet

def make_stairs(n):
    s = 1
    grids = []
    for i in range(n + 1, 1, -1):
        for j in range(i - 1):
            a, b, c = s + j, s + j + 1, s + i + j
            grids.extend([(a, b), (a, c)])
        s += i
    return grids

def A(n, m):
    universe = make_stairs(n)
    GraphSet.set_universe(universe)
    start, goal = n + 1, (n + 1) * (n + 2) // 2
    paths = GraphSet.paths(start, goal)
    return paths.len(2 * (n + m)).len()

cnt = 1
for i in range(1, 15):
    for j in range((i - 1) * (i - 1) // 4 + 1):
        print(str(cnt) + ' ' + str(A(i, j)))
        cnt += 1
