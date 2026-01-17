# a(n) = 8*a(n-1) - 28*a(n-2) + 58*a(n-3) - 78*a(n-4) + 68*a(n-5) - 37*a(n-6) + 10*a(n-7) - a(n-8).
def a(n)
  ary = [1, 8, 36, 122, 354, 948, 2447, 6210]
  (8..n).each{|i|
    ary << 8*ary[-1] - 28*ary[-2] + 58*ary[-3] - 78*ary[-4] + 68*ary[-5] - 37*ary[-6] + 10*ary[-7] - ary[-8]
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