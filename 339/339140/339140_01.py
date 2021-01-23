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
        grids.append((i + k - 1, i))
    return grids

def A339140(n):
    universe = make_CnXPk(n, n)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles()
    return cycles.len()

print([A339140(n) for n in range(3, 9)])
# print(A339140(6))
# print(A339140(7))
# print(A339140(8))
# print(A339140(9))
print(A339140(10))
print(A339140(12))
# print(A339140(14))
# print(A339140(16))


