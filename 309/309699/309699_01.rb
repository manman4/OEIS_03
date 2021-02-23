def A309699(n)
  ary = [5]
  a = 5
  n.times{|i|
    b = (a + a ** 5 - 5) % (6 ** (i + 2))
    ary << (b - a) / (6 ** (i + 1))
    a = b
  }
  ary
end

m = 10100
ary = A309699(m)
(0..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
