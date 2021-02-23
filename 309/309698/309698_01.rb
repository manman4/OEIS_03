def A309698(n)
  ary = [3]
  a = 3
  n.times{|i|
    b = (a + a ** 3 - 3) % (4 ** (i + 2))
    ary << (b - a) / (4 ** (i + 1))
    a = b
  }
  ary
end

m = 10100
ary = A309698(m)
(0..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
