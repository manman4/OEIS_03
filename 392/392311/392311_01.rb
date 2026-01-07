# a(n) = 6*a(n-2) - 15*a(n-4) + 2*a(n-5) + 20*a(n-6) - 6*a(n-7) - 15*a(n-8) + 6*a(n-9) + 5*a(n-10) - 2*a(n-11) - a(n-12).
def a(n)
ary = [1, 0, 2, 0, 3, 4, 4, 20, 5, 60, 13, 140]
  (12..n).each{|i|
    ary << 6*ary[-2] - 15*ary[-4] + 2*ary[-5] + 20*ary[-6] - 6*ary[-7] - 15*ary[-8] + 6*ary[-9] + 5*ary[-10] - 2*ary[-11] - ary[-12]
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