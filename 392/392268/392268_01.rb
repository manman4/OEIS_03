# a(n) = 4*a(n-3) + 2*a(n-5) - 6*a(n-6) - 4*a(n-8) + 4*a(n-9) - a(n-10) + 2*a(n-11) - a(n-12).
def a(n)
  ary = [1, 0, 0, 2, 0, 3, 3, 0, 12, 4, 5, 30]
  (12..n).each{|i|
    ary << 4*ary[-3] + 2*ary[-5] - 6*ary[-6] - 4*ary[-8] + 4*ary[-9] - ary[-10] + 2*ary[-11] - ary[-12]
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