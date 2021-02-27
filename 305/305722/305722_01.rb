def A305722(n)
  a, b, c, d, e, f, g, h, i = 1, 129, 2945, 29953, 187137, 845185, 3032705, 9173505, 24331777
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i = b, c, d, e, f, g, h, i, 9*i - 36*h + 84*g - 126*f + 126*e - 84*d + 36*c  - 9*b + a
    ary << a
  end
  ary
end
n = 10000
ary = A305722(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
