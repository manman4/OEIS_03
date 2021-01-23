# Using graphillion
from graphillion import GraphSet

def make_T_nk(n, k):
    grids = []
    for i in range(1, k + 1):
        for j in range(1, n):
            grids.append((i + (j - 1) * k, i + j * k))
            if i < k:
                grids.append((i + (j - 1) * k, i + j * k + 1))
    for i in range(1, k * n, k):
        for j in range(1, k):
            grids.append((i + j - 1, i + j))
    return grids

def A339849(n, k):
    universe = make_T_nk(n, k)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles(is_hamilton=True)
    return cycles.len()

def A339852(n):
    return A339849(5, n)

# print([A339852(n) for n in range(2, 21)])

for i in range(2, 501):
    print(str(i) + ' ' + str(A339852(i)))
