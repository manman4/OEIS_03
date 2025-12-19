# a(n) = 6*a(n-1) - 9*a(n-2) + 12*a(n-4) + 36*a(n-5) - 36*a(n-8).
def a(n)
  ary = [1, 3, 9, 27, 87, 351, 1539, 6723]
  (8..n).each{|i|
    ary << 6*ary[-1] - 9*ary[-2] + 12*ary[-4] + 36*ary[-5] - 36*ary[-8]
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