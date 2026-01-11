# a(n) = 2*a(n-2) + 6*a(n-3) - a(n-4) + 3*a(n-5) - 3*a(n-6) + a(n-7).
def a(n)
  ary = [1, 0, 1, 3, 1, 15, 16]
  (7..n).each{|i|
    ary << 2*ary[-2] + 6*ary[-3] - ary[-4] + 3*ary[-5] - 3*ary[-6] + ary[-7]
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