def A309600(n)
  ary = [7]
  a = 7
  n.times{|i|
    b = (a + 3 * (9 * a ** 3 - 17)) % (10 ** (i + 2))
    ary << (b - a) / (10 ** (i + 1))
    a = b
  }
  ary
end

p A309600(100)
