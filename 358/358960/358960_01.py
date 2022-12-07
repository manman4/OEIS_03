# Using graphillion
from graphillion import GraphSet


def make_tetrahedral_graph():
    return [
        (1, 2), (1, 3), (1, 4),
        (2, 3), (3, 4), (4, 2),
    ]


def make_cubical_graph():
    return [
        (1, 2), (2, 3), (3, 4), (4, 1),
        (5, 6), (6, 7), (7, 8), (8, 5),
        (1, 5), (2, 6), (3, 7), (4, 8),
    ]


def make_octahedral_graph():
    return [
        (1, 2), (2, 3), (3, 1),
        (4, 5), (5, 6), (6, 4),
        (4, 3), (4, 1),
        (5, 1), (5, 2),
        (6, 2), (6, 3),
    ]


def make_dodecahedral_graph():
    return [
        ( 1,  2), ( 2,  3), ( 3,  4), ( 4,  5), ( 5,  1),
        ( 6,  1), ( 7,  2), ( 8,  3), ( 9,  4), (10,  5),
        (11, 10), (11,  6),
        (12,  6), (12,  7),
        (13,  7), (13,  8),
        (14,  8), (14,  9),
        (15,  9), (15, 10),
        (16, 11), (17, 12), (18, 13), (19, 14), (20, 15),
        (16, 17), (17, 18), (18, 19), (19, 20), (20, 16),
    ]


def make_icosahedral_graph():
    return [
        ( 1,  2), ( 2,  3), ( 3,  1),
        ( 4,  3), ( 4,  1),
        ( 5,  4), ( 5,  1), ( 5,  6),
        ( 6,  1), ( 6,  2),
        ( 7,  6), ( 7,  2), ( 7,  8),
        ( 8,  2), ( 8,  3),
        ( 9,  8), ( 9,  3), ( 9,  4),
        (10, 11), (11, 12), (12, 10),
        (10,  9), (10,  4), (10,  5),
        (11,  5), (11,  6), (11,  7),
        (12,  7), (12,  8), (12,  9),
    ]


def make_universe(n):
    if n == 4:
        return make_tetrahedral_graph()
    elif n == 6:
        return make_cubical_graph()
    elif n == 8:
        return make_octahedral_graph()
    elif n == 12:
        return make_dodecahedral_graph()
    elif n == 20:
        return make_icosahedral_graph()


def spanning_tree(n):
    universe = make_universe(n)
    GraphSet.set_universe(universe)
    spanning_trees = GraphSet.trees(is_spanning=True)
    return spanning_trees.len()


def directed_Hamiltonian_cycle(n):
    universe = make_universe(n)
    GraphSet.set_universe(universe)
    cycles = GraphSet.cycles(is_hamilton=True)
    return 2 * cycles.len()


def directed_Hamiltonian_path(n):
    universe = make_universe(n)
    # V-E+F=2
    v = len(universe) + 2 - n
    GraphSet.set_universe(universe)
    s = 0
    for goal in range(1, v + 1):
        paths = GraphSet.paths(1, goal, is_hamilton=True)
        s += paths.len()
    return v * s


platonic_graph_info = [4, 6, 8, 12, 20]

# A343213
print([spanning_tree(i) for i in platonic_graph_info])

# A268283
print([directed_Hamiltonian_cycle(i) for i in platonic_graph_info])

# A358960
print([directed_Hamiltonian_path(i) for i in platonic_graph_info])
