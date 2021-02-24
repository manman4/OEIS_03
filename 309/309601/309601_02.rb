def A309601(n)
  ary = [3]
  a = 3
  n.times{|i|
    b = (a + 3 * (9 * a ** 3 - 53)) % (10 ** (i + 2))
    ary << (b - a) / (10 ** (i + 1))
    a = b
  }
  ary
end

p A309601(100)
