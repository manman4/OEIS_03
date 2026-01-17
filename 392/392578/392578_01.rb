# a(n) = 8*a(n-1) - 22*a(n-2) + 26*a(n-3) - 17*a(n-4) + 6*a(n-5) - a(n-6).
def a(n)
  ary = [1, 8, 42, 186, 755, 2910]
  (6..n).each{|i|
    ary << 8*ary[-1] - 22*ary[-2] + 26*ary[-3] - 17*ary[-4] + 6*ary[-5] - ary[-6]
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