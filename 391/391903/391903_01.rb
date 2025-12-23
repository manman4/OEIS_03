# a(n) = 3*a(n-2) + 3*a(n-3) - 3*a(n-4) + 21*a(n-5) - 2*a(n-6) + 3*a(n-7) + 3*a(n-8) + a(n-9).
def a(n)
  ary = [1, 0, 1, 1, 1, 20, 2, 84, 85]
  (9..n).each{|i|
    ary << 3*ary[-2] + 3*ary[-3] - 3*ary[-4] + 21*ary[-5] - 2*ary[-6] + 3*ary[-7] + 3*ary[-8] + ary[-9]
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