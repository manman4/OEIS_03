# a(n) = 6*a(n-2) - 15*a(n-4) + 3*a(n-5) + 20*a(n-6) - 12*a(n-7) - 15*a(n-8) + 18*a(n-9) + 3*a(n-10) - 12*a(n-11) + 5*a(n-12) + 3*a(n-13) - 3*a(n-14) + a(n-15).
def a(n)
  ary = [1, 0, 3, 0, 6, 6, 10, 30, 15, 90, 36, 210, 133, 420, 456]
  (15..n).each{|i|
    ary << 6*ary[-2] - 15*ary[-4] + 3*ary[-5] + 20*ary[-6] - 12*ary[-7] - 15*ary[-8] + 18*ary[-9] + 3*ary[-10] - 12*ary[-11] + 5*ary[-12] + 3*ary[-13] - 3*ary[-14] + ary[-15]
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