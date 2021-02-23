def A309724(n)
  ary = [7]
  a = 7
  n.times{|i|
    b = (a +  7 * (7 * a ** 7 - 1)) % (8 ** (i + 2))
    ary << (b - a) / (8 ** (i + 1))
    a = b
  }
  ary
end

p A309724(100)
