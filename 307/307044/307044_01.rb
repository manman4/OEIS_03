def A(n)
  a, b, c, d, e, f, g, h = 1, 1, 1, 1, 1, 1, 1, 1
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h = b, c, d, e, f, g, h, 8 * h - 28 * g + 56 * f - 70 * e + 56 * d - 28 * c + 8 * b - 2 * a
    ary << a
  end
  ary
end

n = 3000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
