def A(n)
  a, b, c, d, e, f = 1, 2, 5, 16, 46, 128
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f = b, c, d, e, f, 2 * f + e + 4 * d + c - 2 * b - a
    ary << a
  end
  ary
end

n = 2000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
