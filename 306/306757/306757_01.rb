def A(n)
  a, b, c, d, e, f, g, h, i = 9, 0, 0, 0, 0, 0, 0, 0, 8
  ary = [9]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i = b, c, d, e, f, g, h, i, a + b
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
