# a(n) = 2*a(n-1) - 2*a(n-2) + 2*a(n-3) - 2*a(n-4) + 2*a(n-5) + a(n-6) - a(n-7) + a(n-8) - a(n-9) + a(n-10) - a(n-11) + a(n-12) - a(n-13) + a(n-14) - a(n-15) + a(n-16).
def a(n)
  ary = [1, 1, 1, 1, 1, 1, 3, 6, 9, 12, 15, 18, 27, 45, 72, 108]
  (16..n).each{|i|
    ary << 2*ary[-1] - 2*ary[-2] + 2*ary[-3] - 2*ary[-4] + 2*ary[-5] + ary[-6] - ary[-7] + ary[-8] - ary[-9] + ary[-10] - ary[-11] + ary[-12] - ary[-13] + ary[-14] - ary[-15] + ary[-16]
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