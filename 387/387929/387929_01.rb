# 6*n*(2*n-1)*(25*n-34)*a(n) = (3275*n^3-7729*n^2+5156*n-972)*a(n-1) - 16*(n-1)*(2*n-3)*(25*n-9)*a(n-2) for n > 1. 
def A(n)
  ary = [1, 5]
  (2..n).each{|i|
    ary << ((3275*i**3-7729*i**2+5156*i-972)*ary[-1] - 16*(i-1)*(2*i-3)*(25*i-9)*ary[-2]) / (6*i*(2*i-1)*(25*i-34))
  }
  ary
end

n = 600
ary = A(n)
(0..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}