# a(n) = 10*a(n-1) - 37*a(n-2) + 68*a(n-3) - 78*a(n-4) + 58*a(n-5) - 28*a(n-6) + 8*a(n-7) - a(n-8).
def a(n)
  ary = [1, 10, 63, 328, 1551, 6936, 29915, 125784]
  (8..n).each{|i|
    ary << 10*ary[-1] - 37*ary[-2] + 68*ary[-3] - 78*ary[-4] + 58*ary[-5] - 28*ary[-6] + 8*ary[-7] - ary[-8]
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