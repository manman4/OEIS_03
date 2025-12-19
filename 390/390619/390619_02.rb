# aa(n) = 4*a(n-3) + 12*a(n-4) - 4*a(n-6) + 24*a(n-7) - 36*a(n-8).
def a(n)
  ary = [1, 0, 0, 2, 6, 0, 4, 72]
  (8..n).each{|i|
    ary << 4*ary[-3] + 12*ary[-4] - 4*ary[-6] + 24*ary[-7] - 36*ary[-8]
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