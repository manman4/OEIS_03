# a(n) = 6*a(n-2) - 15*a(n-4) + 20*a(n-6) + 2*a(n-7) - 15*a(n-8) - 6*a(n-9) + 6*a(n-10) + 6*a(n-11) - a(n-12) - 2*a(n-13) - a(n-14).
def a(n)
  ary = [1, 0, 2, 0, 3, 0, 4, 4, 5, 20, 6, 60, 7, 140]
  (14..n).each{|i|
    ary << 6*ary[-2] - 15*ary[-4] + 20*ary[-6] + 2*ary[-7] - 15*ary[-8] - 6*ary[-9] + 6*ary[-10] + 6*ary[-11] - ary[-12] - 2*ary[-13] - ary[-14]
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