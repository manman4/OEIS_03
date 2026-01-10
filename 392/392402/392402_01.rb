# a(n) = 3*a(n-1) - 3*a(n-2) + 3*a(n-3) + 6*a(n-4) - a(n-6).
def a(n)
  ary = [1, 1, 1, 2, 11, 36]
  (6..n).each{|i|
    ary << 3*ary[-1] - 3*ary[-2] + 3*ary[-3] + 6*ary[-4] - ary[-6]
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