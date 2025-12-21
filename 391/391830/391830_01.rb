# a(n) = 4*a(n-2) + 4*a(n-3) - 6*a(n-4) - 4*a(n-5) - 2*a(n-6) - 4*a(n-7) - 5*a(n-8) + 8*a(n-9) - 6*a(n-10) + 4*a(n-11) - a(n-12).
def a(n)
  ary = [1, 0, 3, 3, 6, 20, 16, 70, 85, 190, 399, 565]
  (12..n).each{|i|
    ary << 4*ary[-2] + 4*ary[-3] - 6*ary[-4] - 4*ary[-5] - 2*ary[-6] - 4*ary[-7] - 5*ary[-8] + 8*ary[-9] - 6*ary[-10] + 4*ary[-11] - ary[-12]
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