def A(n)
  a, b, c, d, e, f, g, h, i, j = 1,8953,856945,17538157,167729959,1018872811,4577127763,16581420835,51125645317,139071924069
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i, j = b, c, d, e, f, g, h, i, j, 10*j - 45*i + 120*h - 210*g + 252*f - 210*e + 120*d - 45*c + 10*b - a
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
