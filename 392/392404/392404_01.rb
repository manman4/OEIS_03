# a(n) = 3*a(n-1) - 3*a(n-2) + a(n-3) + 2*a(n-4) + 6*a(n-5) - a(n-8).
def a(n)
  ary = [1, 1, 1, 1, 2, 11, 36, 85]
  (8..n).each{|i|
    ary << 3*ary[-1] - 3*ary[-2] + ary[-3] + 2*ary[-4] + 6*ary[-5] - ary[-8]
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