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

def A340005(n):

    if n == 0: return 1

    universe = make_stairs(n)

    GraphSet.set_universe(universe)

    start, goal = n + 1, (n + 1) * (n + 2) // 2

    paths = GraphSet.paths(start, goal)

    return paths.len()

print([A340005(n) for n in range(15)])

for i in range(51):
    print(str(i) + ' ' + str(A340005(i)))

