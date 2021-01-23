# Using graphillion
from graphillion import GraphSet

def make_CnXPk(n, k):
    grids = []
    for i in range(1, k + 1):
        for j in range(1, n):
            grids.append((i + (j - 1) * k, i + j * k))
        grids.append((i + (n - 1) * k, i))
    for i in range(1, k * n, k):
        for j in range(1, k):
            grids.append((i + j - 1, i + j))
    return grids

def A339143(n):
    universe = make_CnXPk(6, n)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    return cycles.len()

# print([A339143(n) for n in range(1, 20)])
for n in range(1, 501):
    print(str(n) + " " + str(A339143(n)))
