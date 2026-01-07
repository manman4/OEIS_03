# a(n) = 9*a(n-1) - 36*a(n-2) + 84*a(n-3) - 123*a(n-4) + 108*a(n-5) - 39*a(n-6) - 24*a(n-7) + 33*a(n-8) - 8*a(n-9) - 6*a(n-10) + 3*a(n-11) + a(n-12).
def a(n)
  ary = [1, 3, 6, 10, 25, 81, 238, 596, 1333, 2827, 5946, 12618]
  (12..n).each{|i|
    ary << 9*ary[-1] - 36*ary[-2] + 84*ary[-3] - 123*ary[-4] + 108*ary[-5] - 39*ary[-6] - 24*ary[-7] + 33*ary[-8] - 8*ary[-9] - 6*ary[-10] + 3*ary[-11] + ary[-12]
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