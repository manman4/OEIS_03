def h(coords):

  def anyhex(i, j):

    c = [x in coords for x in [(i-1, j), (i, j+1), (i+1, j+1), (i+1, j), (i, j-1), (i-1, j-1)]]

    return any(map(lambda x, y: x and y, c, c[1:] + c[:1]))

  return all(anyhex(*z) for z in coords)

def g(coords):

  if not coords: return 1

  #if not h(coords): return 0

  i, j = min(coords)

  if (i+1, j+1) not in coords: return 0

  cases = 0

  if (i+1, j) in coords: cases += g(coords - {(i, j), (i+1, j), (i+1, j+1)})

  if (i, j+1) in coords: cases += g(coords - {(i, j), (i, j+1), (i+1, j+1)})

  return cases

def f(n):

  coords = {(i, j) for i in range(n) for j in range(i+1)}

  #if n%12 not in [0, 2, 9, 11]: return 0

  print(n, g(coords) if n%12 in [0, 2, 9, 11] else 0)

[f(x) for x in range(21)]