def A(n)
  a, b, c, d, e, f, g = 1, 1, 1, 1, 1, 1, 1
  ary = [1, 1]
  while ary.size <= n
    a, b, c, d, e, f, g = b, c, d, e, f, g, 8 * g - 28 * f + 56 * e - 70 * d + 56 * c - 28 * b + 8 * a
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
