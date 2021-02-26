def A(n)
  a, b, c, d, e, f = 1, 6, 21, 56, 126, 252
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f = b, c, d, e, f, 6 * f - 15 * e + 20 * d - 15 * c + 6 * b - 2 * a
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
