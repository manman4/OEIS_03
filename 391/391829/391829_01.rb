# a(n) = 4*a(n-1) - 2*a(n-2) - 11*a(n-4) - 2*a(n-6) + 4*a(n-7) - a(n-8).
def a(n)
  ary = [1, 3, 9, 30, 91, 271, 801, 2330]
  (8..n).each{|i|
    ary << 4*ary[-1] - 2*ary[-2] - 11*ary[-4] - 2*ary[-6] + 4*ary[-7] - ary[-8]
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