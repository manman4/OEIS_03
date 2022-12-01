# Using graphillion
from graphillion import GraphSet

def make_PnXPk(n, k):
    grids = []
    for i in range(1, k + 1):
        for j in range(1, n):
            grids.append((i + (j - 1) * k, i + j * k))
    for i in range(1, k * n, k):
        for j in range(1, k):
            grids.append((i + j - 1, i + j))
    return grids

def A(start, goal, n, k):
    universe = make_PnXPk(n, k)
    GraphSet.set_universe(universe)
    paths = GraphSet.paths(start, goal, is_hamilton=True)
    return paths.len()

def A332307(n, k):
    m = k * n
    s = 0
    for i in range(1, m):
        for j in range(i + 1, m + 1):
            s += A(i, j, n, k)
    return s

def A358795(n):
    if n % 2 == 0:
        return A332307(n, 8)
    else:
        return A332307(8, n)

for n in range(1, 25):
    print(A358795(n)) 
