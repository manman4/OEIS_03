# a(n) = 4*a(n-1) + 2*a(n-2) - 4*a(n-3) - 33*a(n-4) - 8*a(n-5) + 8*a(n-6) + 32*a(n-7) - 16*a(n-8).
def a(n)
  ary = [1, 2, 15, 64, 233, 926, 3411, 12380]
  (8..n).each{|i|
    ary << 4*ary[-1] + 2*ary[-2] - 4*ary[-3] - 33*ary[-4] - 8*ary[-5] + 8*ary[-6] + 32*ary[-7] - 16*ary[-8]
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