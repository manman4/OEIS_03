# a(n) = 2*a(n-3) - a(n-6) + a(n-7).
def a(n)
  ary = [1, 0, 0, 2, 0, 0, 3]
  (7..n).each{|i|
    ary << 2*ary[-3] - ary[-6] + ary[-7]
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