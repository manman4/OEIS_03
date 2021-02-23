def A309646(n)
  ary = [1]
  a = 1
  n.times{|i|
    b = (a + 7 * (9 * a ** 3 + 71)) % (10 ** (i + 2))
    ary << (b - a) / (10 ** (i + 1))
    a = b
  }
  ary
end

n = 10000
ary = A309646(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
