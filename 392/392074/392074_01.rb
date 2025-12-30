# a(n) = 8*a(n-3) - 24*a(n-6) + 32*a(n-9) - 16*a(n-12) + 48*a(n-13).
def a(n)
  ary = [1, 0, 0, 2, 0, 0, 4, 0, 0, 8, 0, 0, 16]
  (13..n).each{|i|
    ary << 8*ary[-3] - 24*ary[-6] + 32*ary[-9] - 16*ary[-12] + 48*ary[-13]
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