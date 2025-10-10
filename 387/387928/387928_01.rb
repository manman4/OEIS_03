# 2*n*(2*n-1)*(11*n-14)*a(n) = (649*n^3-1475*n^2+964*n-180)*a(n-1) + 16*(n-1)*(2*n-3)*(11*n-3)*a(n-2) for n > 1.
def A(n)
  ary = [1, 7]
  (2..n).each{|i|
    ary << ((649*i**3-1475*i**2+964*i-180)*ary[-1] + 16*(i-1)*(2*i-3)*(11*i-3)*ary[-2]) / (2*i*(2*i-1)*(11*i-14))
  }
  ary
end

n = 500
ary = A(n)
(0..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}