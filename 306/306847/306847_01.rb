def A(n)
  a, b, c, d, e = 1, 1, 1, 1, 1
  ary = [1, 1]
  while ary.size <= n
    a, b, c, d, e = b, c, d, e, 6 * e - 15 * d + 20 * c - 15 * b + 6 * a
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
