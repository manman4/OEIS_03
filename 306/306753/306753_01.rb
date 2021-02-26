def A(n)
  a, b, c, d, e, f, g, h, i, j = 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i, j = b, c, d, e, f, g, h, i, j, 9 * j - 36 * i + 84 * h - 126 * g + 126 * f - 84 * e + 36 * d - 9 * c + b + a
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
