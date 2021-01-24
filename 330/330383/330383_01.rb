def A(n)
  a, b, c, d, e, f, g, h = 1, 1, 1, 1, 1, 1, 1, 1
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h = b, c, d, e, f, g, h, (b * h + c * g - d * f + e * e) / a
    ary << a
  end
  ary
end

n = 400
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}