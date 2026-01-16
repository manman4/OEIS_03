# a(n) = 3*a(n-1) - 3*a(n-2) + a(n-3) + a(n-7).
def a(n)
  ary = [1, 3, 6, 10, 15, 21, 28]
  (7..n).each{|i|
    ary << 3*ary[-1] - 3*ary[-2] + ary[-3] + ary[-7]
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