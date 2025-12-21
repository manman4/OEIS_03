# a(n) = 4*a(n-1) - 6*a(n-2) + 8*a(n-3) - 5*a(n-4) - 4*a(n-5) - 2*a(n-6) - 4*a(n-7) - 6*a(n-8) + 4*a(n-9) + 4*a(n-10) - a(n-12).
def a(n)
  ary = [1, 3, 6, 13, 35, 91, 214, 491, 1151, 2711, 6290, 14427]
  (12..n).each{|i|
    ary << 4*ary[-1] - 6*ary[-2] + 8*ary[-3] - 5*ary[-4] - 4*ary[-5] - 2*ary[-6] - 4*ary[-7] - 6*ary[-8] + 4*ary[-9] + 4*ary[-10] - ary[-12]
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