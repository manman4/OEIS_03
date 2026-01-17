# a(n) = 8*a(n-1) - 26*a(n-2) + 48*a(n-3) - 59*a(n-4) + 48*a(n-5) - 26*a(n-6) + 8*a(n-7) - a(n-8).
def a(n)
  ary = [1, 8, 38, 144, 489, 1568, 4858, 14696]
  (8..n).each{|i|
    ary << 8*ary[-1] - 26*ary[-2] + 48*ary[-3] - 59*ary[-4] + 48*ary[-5] - 26*ary[-6] + 8*ary[-7] - ary[-8]
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