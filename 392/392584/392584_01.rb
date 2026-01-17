# a(n) = 6*a(n-1) - 15*a(n-2) + 22*a(n-3) - 21*a(n-4) + 12*a(n-5) - 4*a(n-6).
def a(n)
  ary = [1, 6, 21, 58, 144, 342]
  (6..n).each{|i|
    ary << 6*ary[-1] - 15*ary[-2] + 22*ary[-3] - 21*ary[-4] + 12*ary[-5] - 4*ary[-6]
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