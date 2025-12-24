# a(n) = 6*a(n-1) - 15*a(n-2) + 23*a(n-3) - 27*a(n-4) + 24*a(n-5) - 16*a(n-6) + 9*a(n-7) - 3*a(n-8) + a(n-9).
def a(n)
  ary = [1, 3, 6, 16, 45, 111, 253, 561, 1221]
  (9..n).each{|i|
    ary << 6*ary[-1] - 15*ary[-2] + 23*ary[-3] - 27*ary[-4] + 24*ary[-5] - 16*ary[-6] + 9*ary[-7] - 3*ary[-8] + ary[-9]
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