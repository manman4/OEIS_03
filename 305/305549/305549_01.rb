def A305549(n)
  a, b, c, d, e, f, g = 1, 73, 985, 6321, 26577, 85305, 227305
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g = b, c, d, e, f, g, 7*g - 21*f + 35*e - 35*d + 21*c - 7*b + a
    ary << a
  end
  ary
end
n = 10000
ary = A305549(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
