def A(n)
  a, b, c, d, e, f, g, h = 8, 0, 0, 0, 0, 0, 0, 7
  ary = [8]
  while ary.size <= n
    a, b, c, d, e, f, g, h = b, c, d, e, f, g, h, a + b
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
