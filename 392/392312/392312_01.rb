# a(n) = 6*a(n-3) - 15*a(n-6) + 2*a(n-8) + 20*a(n-9) - 6*a(n-11) - 15*a(n-12) + 6*a(n-14) + 6*a(n-15) - a(n-16) - 2*a(n-17) - a(n-18).
def a(n)
ary = [1, 0, 0, 2, 0, 0, 3, 0, 4, 4, 0, 20, 5, 0, 60, 6, 7, 140]
  (18..n).each{|i|
    ary << 6*ary[-3] - 15*ary[-6] + 2*ary[-8] + 20*ary[-9] - 6*ary[-11] - 15*ary[-12] + 6*ary[-14] + 6*ary[-15] - ary[-16] - 2*ary[-17] - ary[-18]
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