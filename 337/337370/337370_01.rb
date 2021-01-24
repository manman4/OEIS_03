# def A(k, n)
#   a = 1
#   b = k + 6
#   ary = [a]
#   m = 1
#   while m <= n
#     m += 1
#     a, b = b, ((4*m-1)*(4*(k+4)*m*m-2*(k+4)*m-k-2)*b - (k-4)**2 * (m-1)*(2*m-1)*(4*m+1)*a)/(m*(2*m+1)*(4*m-3))
#     ary << a
#   end
#   ary
# end

# (-4..4).each{|i| p [i, A(i, 20)]}

def A(n)
  a = 1
  b = 8
  ary = [a]
  m = 1
  while m <= n
    m += 1
    a, b = b, ((4*m-1)*(24*m*m-12*m-4)*b - 4 * (m-1)*(2*m-1)*(4*m+1)*a)/(m*(2*m+1)*(4*m-3))
    ary << a
  end
  ary
end

n = 100
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
