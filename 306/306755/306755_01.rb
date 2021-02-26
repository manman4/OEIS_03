def A(n)
  a, b, c, d, e, f, g = 7, 0, 0, 0, 0, 0, 6
  ary = [7]
  while ary.size <= n
    a, b, c, d, e, f, g = b, c, d, e, f, g, a + b
    ary << a
  end
  ary
end

n = 10000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
