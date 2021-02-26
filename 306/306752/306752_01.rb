def A(n)
  a, b, c, d, e, f, g, h, i = 1, 1, 1, 1, 1, 1, 1, 1, 1
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i = b, c, d, e, f, g, h, i, 8 * i - 28 * h + 56 * g - 70 * f + 56 * e - 28 * d + 8 * c - b + a
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
