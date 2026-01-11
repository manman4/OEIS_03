# a(n) = 6*a(n-2) + 3*a(n-3) - 3*a(n-4) + 3*a(n-5) - a(n-6).
def a(n)
  ary = [1, 0, 3, 2, 15, 24]
  (6..n).each{|i|
    ary << 6*ary[-2] + 3*ary[-3] - 3*ary[-4] + 3*ary[-5] - ary[-6]
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