# a(n) = 6*a(n-3) + 3*a(n-5) - 15*a(n-6) - 12*a(n-8) + 20*a(n-9) - 3*a(n-10) + 18*a(n-11) - 15*a(n-12) + 6*a(n-13) - 12*a(n-14) + 7*a(n-15) - 3*a(n-16) + 3*a(n-17) - a(n-18).
def a(n)
  ary = [1, 0, 0, 3, 0, 6, 6, 0, 30, 10, 15, 90, 15, 105, 210, 49, 420, 420]
  (18..n).each{|i|
    ary << 6*ary[-3] + 3*ary[-5] - 15*ary[-6] - 12*ary[-8] + 20*ary[-9] - 3*ary[-10] + 18*ary[-11] - 15*ary[-12] + 6*ary[-13] - 12*ary[-14] + 7*ary[-15] - 3*ary[-16] + 3*ary[-17] - ary[-18]
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