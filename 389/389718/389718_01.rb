# a(n) = 5*a(n-3) - 10*a(n-6) + 10*a(n-9) - 5*a(n-12) + a(n-14) + a(n-15).
def a(n)
  ary = [1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1]
  (15..n).each{|i|
    ary << 5*ary[-3] - 10*ary[-6] + 10*ary[-9] - 5*ary[-12] + ary[-14] + ary[-15]
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