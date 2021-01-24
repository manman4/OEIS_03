def A(n)
  ary = [1]
  a, b, c, d = 1, 7, 71, 769
  while ary.size <= n
    a, b, c, d = b, c, d, 14 * d - 34 * c + 14 * b - a
    ary << a
  end
  ary
end

p A(20)