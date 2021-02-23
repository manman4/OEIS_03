def A309723(n)
  ary = [5]
  a = 5
  n.times{|i|
    b = (a +  5 * (5 * a ** 5 - 1)) % (6 ** (i + 2))
    ary << (b - a) / (6 ** (i + 1))
    a = b
  }
  ary
end

p A309723(100)
