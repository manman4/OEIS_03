# a(n) = 2*a(n-3) + 6*a(n-4) - a(n-6) + 3*a(n-7) - 3*a(n-8) + a(n-9).
def a(n)
  ary = [1, 0, 0, 1, 3, 0, 1, 15, 15]
  (9..n).each{|i|
    ary << 2*ary[-3] + 6*ary[-4] - ary[-6] + 3*ary[-7] - 3*ary[-8] + ary[-9]
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