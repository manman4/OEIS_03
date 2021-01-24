# def A(k, n)
#   a = 1
#   b = k + 2
#   ary = [a]
#   m = 1
#   while m <= n
#     m += 1
#     a, b = b, ((4*m-3)*((4*k+16)*m*m-(6*k+24)*m+k+6)*b - (k-4)**2 * (m-1)*(4*m-1)*(2*m-3)*a)/(m*(2*m-1)*(4*m-5))
#     ary << a
#   end
#   ary
# end

def A(n)
  a = 1
  b = -2
  ary = [a]
  m = 1
  while m <= n
    m += 1
    a, b = b, ((4*m-3)*2*b - 64 * (m-1)*(4*m-1)*(2*m-3)*a)/(m*(2*m-1)*(4*m-5))
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
