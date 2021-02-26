def A(n)
  a, b, c, d, e, f, g, h = 1, 1, 1, 1, 1, 1, 1, 1
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h = b, c, d, e, f, g, h, 7 * h - 21 * g + 35 * f - 35 * e + 21 * d - 7 * c + b + a
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
