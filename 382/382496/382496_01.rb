# a(n) = 4*a(n-3) + 4*a(n-4) - 6*a(n-6) - 4*a(n-7) - 6*a(n-8) + 4*a(n-9) - 4*a(n-10) - 4*a(n-11) + 3*a(n-12) + 4*a(n-13) - 6*a(n-14) + 4*a(n-15) - a(n-16).
def a(n)
  ary = [1, 0, 0, 2, 2, 0, 3, 18, 3, 4, 60, 60, 9, 140, 350, 146]
  (16..n).each{|i|
    ary << 4*ary[-3] + 4*ary[-4] - 6*ary[-6] - 4*ary[-7] - 6*ary[-8] + 4*ary[-9] - 4*ary[-10] - 4*ary[-11] + 3*ary[-12] + 4*ary[-13] - 6*ary[-14] + 4*ary[-15] - ary[-16]
  }
  ary
end

n = 5000
ary = a(n)
(0..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 