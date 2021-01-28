def A322931(n)
  ary = [3]
  a = 3
  n.times{|i|
    b = (a + 5 * (a ** 3 - 3)) % (8 ** (i + 2))
    ary << (b - a) / (8 ** (i + 1))
    a = b
  }
  ary
end

m = 10100
n = 10000
ary = A322931(m)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
