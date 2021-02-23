def A309645(n)
  ary = [9]
  a = 9
  n.times{|i|
    b = (a + 7 * (9 * a ** 3 - 11)) % (10 ** (i + 2))
    ary << (b - a) / (10 ** (i + 1))
    a = b
  }
  ary
end

p A309645(100)
n = 10
ary = A309645(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
