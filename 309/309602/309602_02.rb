def A309602(n)
  ary = [9]
  a = 9
  n.times{|i|
    b = (a + 7 * (9 * a ** 3 - 71)) % (10 ** (i + 2))
    ary << (b - a) / (10 ** (i + 1))
    a = b
  }
  ary
end

n = 10000
ary = A309602(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
