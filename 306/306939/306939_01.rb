def A(n)
  a, b, c, d, e, f, g, h, i = 1, 9, 45, 165, 495, 1287, 3003, 6435, 12870
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i = b, c, d, e, f, g, h, i, 9 * i - 36 * h + 84 * g - 126 * f + 126 * e - 84 * d + 36 * c - 9 * b + 2 * a
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
