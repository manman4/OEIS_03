# a(n) = 4*a(n-1) - 6*a(n-2) + 6*a(n-3) - 5*a(n-4) + 2*a(n-5) - a(n-6).
def a(n)
  ary = [1, 2, 3, 7, 17, 36]
  (6..n).each{|i|
    ary << 4*ary[-1] - 6*ary[-2] + 6*ary[-3] - 5*ary[-4] + 2*ary[-5] - ary[-6]
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