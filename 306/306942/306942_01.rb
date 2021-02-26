def A(n)
  a, b, c, d, e, f, g, h = 1, 9, 45, 165, 495, 1287, 3003, 6435
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h = b, c, d, e, f, g, h, 9 * h - 36 * g + 84 * f - 126 * e + 126 * d - 84 * c + 36 * b - 9 * a
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
