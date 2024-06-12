# a(n) = a(n-1) + 3*a(n-2) - 3*a(n-4) + a(n-6) for n > 5.
def A(n)
  a, b, c, d, e, f = 1, 1, 2, 5, 9, 21
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f = b, c, d, e, f, f + 3 * e - 3 * c + a
    ary << a
  end
  ary
end

n = 1100
m = 1000
ary = A(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}