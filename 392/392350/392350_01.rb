# a(n) = 2*a(n-1) - 2*a(n-2) + 2*a(n-3) + a(n-4) - a(n-5) + a(n-6) - a(n-7) + a(n-8) - a(n-9) + a(n-10).
def a(n)
  ary = [1, 1, 1, 1, 3, 6, 9, 12, 21, 39]
  (10..n).each{|i|
    ary << 2*ary[-1] - 2*ary[-2] + 2*ary[-3] + ary[-4] - ary[-5] + ary[-6] - ary[-7] + ary[-8] - ary[-9] + ary[-10]
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