# a(n) = 12*a(n-1) - 60*a(n-2) + 172*a(n-3) - 336*a(n-4) + 480*a(n-5) - 496*a(n-6) + 384*a(n-7) - 192*a(n-8) + 64*a(n-9).
def a(n)
  ary = [1, 6, 24, 104, 480, 2112, 8752, 34848, 135168]
  (9..n).each{|i|
    ary << 12*ary[-1] - 60*ary[-2] + 172*ary[-3] - 336*ary[-4] + 480*ary[-5] - 496*ary[-6] + 384*ary[-7] - 192*ary[-8] + 64*ary[-9]
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