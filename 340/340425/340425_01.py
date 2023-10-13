from graphillion import GraphSet

def make_OD_nk(n, k):
    n += 1
    k += 1
    grids = []
    s = k * n
    for i in range(1, k * n, k):
        for j in range(1, k):
            a, b = i + j - 1, i + j
            c = s + a
            if i > 1:
                grids.extend([(c - k, a), (c - k, b)])
            if i < k * (n - 1) + 1:
                grids.extend([(c, a), (c, b)])
    return grids

def A340425(n, k):
    universe = make_OD_nk(n, k)
    GraphSet.set_universe(universe)
    spanning_trees = GraphSet.trees(is_spanning=True)
    return spanning_trees.len()

print([A340425(n, n) for n in range(1, 6)])
