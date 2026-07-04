# D-finite with recurrence 3*n*(n-1)*a(n) - 2*(n-1)*(14*n-13)*a(n-1) + (64*n^2 - 192*n + 140)*a(n-2) = 0.
def a(n)
  ary = [1,2]
  (2..n).each{|i|
    ary << -(
      ( -2*(i-1)*(14*i-13)*ary[-1] + (64*i**2 - 192*i + 140)*ary[-2] )
    ) / (3*i*(i-1))
  }
  ary
end

n = 1000
ary = a(n)
(0..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
