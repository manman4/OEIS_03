def A(n)
  a, b, c, d, e, f, g, h, i, j = 10, 0, 0, 0, 0, 0, 0, 0, 0, 9
  ary = [10]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i, j = b, c, d, e, f, g, h, i, j, a + b
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
