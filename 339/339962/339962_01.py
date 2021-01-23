# Using graphillion
from graphillion import GraphSet

def make_T_nk(n, k):
    grids = []
    for i in range(1, n + 1):
        for j in range(1, k):
            grids.append((i + (j - 1) * n, i + j * n))
            if i > 1:
                grids.append((i + (j - 1) * n, i + j * n - 1))
    for i in range(1, k * n, n):
        for j in range(1, n):
            grids.append((i + j - 1, i + j))
    return grids

def A339849(n, k):
    universe = make_T_nk(n, k)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles(is_hamilton=True)
    return cycles.len()

for i in range(2, 15):
    print(str(i) + ' ' + str(A339849(10, i)))

