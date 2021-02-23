def A309641(n)
  ary = [7]
  a = 7
  n.times{|i|
    b = (a + 9 * (3 * a ** 3 + 11)) % (10 ** (i + 2))
    ary << (b - a) / (10 ** (i + 1))
    a = b
  }
  ary
end

n = 10000
ary = A309641(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
