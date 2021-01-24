def search(x, y, n, used)
  return 0 if x < 0 || n <= x || y < 0 || n <= y || used[x + y * n]
  return 1 if x == n - 1 && y == n - 1 && (0..n - 2).all?{|i| used[(n + 1) * i] == true}
  cnt = 0
  used[x + y * n] = true
  @move.each{|mo|
    cnt += search(x + mo[0], y + mo[1], n, used)
  }
  used[x + y * n] = false
  cnt
end

def A(n)
  @move = [[1, 0], [-1, 0], [0, 1], [0, -1]]
  used = Array.new(n * n, false)
  search(0, 0, n, used)
end

def A333455(n)
  (1..n).map{|i| A(i)}
end

p A333455(6)