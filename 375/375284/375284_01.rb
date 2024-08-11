# a(n) = 2*a(n-1) - a(n-2) + 2*a(n-5) + 2*a(n-6) - a(n-10).
def A(n)
  ary = [1]
  a, b, c, d, e, f, g, h, i, j = 1, 1, 1, 1, 1, 2, 7, 16, 29, 46
  while ary.size < n + 1
    a, b, c, d, e, f, g, h, i, j = b, c, d, e, f, g, h, i, j, 2 * j - i + 2 * f + 2 * e - a
    ary << a
  end
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
