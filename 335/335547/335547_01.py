def A(n):
    points = {(x, y) for x in range(n) for y in range(n) if x <= y}
    pieces = []
    for size in range(1, n+1):
        s = {(x, y) for x in range(size) for y in range(size) if x <= y}
        pieces += [{(x+ox, y+oy) for x, y in s} for ox in range(n) for oy in range(n)]
    pieces = [p for p in pieces if p <= points]
    pieces.sort(key=min)
    def f(grid, a, depth=1):
        if not grid: return 1
        if not a: return 0
        b0, b1 = a[0], a[1:]
        if min(grid) != min(b0): return 0
        if b0 <= grid:
            return f(grid - b0, [b for b in b1 if not (b0 & b)], depth+1) + f(grid, b1, depth+1)
        return f(grid, b1, depth+1)
    return f(points, pieces)

for i in range(1, 20):
    print([i, A(i)])