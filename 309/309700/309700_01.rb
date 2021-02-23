def A309700(n)
  ary = [7]
  a = 7
  n.times{|i|
    b = (a + a ** 7 - 7) % (8 ** (i + 2))
    ary << (b - a) / (8 ** (i + 1))
    a = b
  }
  ary
end

m = 10100
ary = A309700(m)
(0..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
