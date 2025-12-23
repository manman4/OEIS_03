# a(n) = 8*a(n-1) - 16*a(n-2) + 16*a(n-3) - 72*a(n-4) + 32*a(n-5) - 64*a(n-6) + 64*a(n-7) - 16*a(n-8).
def a(n)
  ary = [1, 4, 24, 152, 812, 4192, 21312, 106048]
  (8..n).each{|i|
    ary << 8*ary[-1] - 16*ary[-2] + 16*ary[-3] - 72*ary[-4] + 32*ary[-5] - 64*ary[-6] + 64*ary[-7] - 16*ary[-8]
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