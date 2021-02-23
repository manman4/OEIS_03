def A309722(n)
  ary = [3]
  a = 3
  n.times{|i|
    b = (a +  3 * (3 * a ** 3 - 1)) % (4 ** (i + 2))
    ary << (b - a) / (4 ** (i + 1))
    a = b
  }
  ary
end

p A309722(100)
