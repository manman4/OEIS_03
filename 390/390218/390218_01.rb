# a(n) = 4*a(n-2) - 6*a(n-4) + 4*a(n-6) + a(n-7) - a(n-8).
def a(n)
  ary = [1, 0, 2, 0, 3, 0, 4, 1]
  (8..n).each{|i|
    ary << 4*ary[-2] - 6*ary[-4] + 4*ary[-6] + ary[-7] - ary[-8]
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