def A(n)
  a, b, c, d, e, f, g, h, i = 1,3139,180325,2636263,19610233,97464799,370487485,1163205475,3164588407
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i = b, c, d, e, f, g, h, i, 9*i - 36*h + 84*g - 126*f + 126*e - 84*d + 36*c - 9*b + a
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
