# a(n) = 6*a(n-2) + 3*a(n-3) - 15*a(n-4) - 12*a(n-5) + 17*a(n-6) + 18*a(n-7) - 9*a(n-8) - 11*a(n-9) + 3*a(n-10) + 3*a(n-11) - a(n-12).
def a(n)
  ary = [1, 0, 3, 6, 6, 30, 25, 90, 120, 238, 441, 672]
  (12..n).each{|i|
    ary << 6*ary[-2] + 3*ary[-3] - 15*ary[-4] - 12*ary[-5] + 17*ary[-6] + 18*ary[-7] - 9*ary[-8] - 11*ary[-9] + 3*ary[-10] + 3*ary[-11] - ary[-12]
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