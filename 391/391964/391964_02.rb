# a(n) = 8*a(n-1) - 24*a(n-2) + 40*a(n-3) - 48*a(n-4) + 32*a(n-5) - 16*a(n-6).
def a(n)
  ary = [1, 4, 12, 44, 176, 672]
  (6..n).each{|i|
    ary << 8*ary[-1] - 24*ary[-2] + 40*ary[-3] - 48*ary[-4] + 32*ary[-5] - 16*ary[-6]
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