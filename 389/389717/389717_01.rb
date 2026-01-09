# a(n) = 4*a(n-3) - 6*a(n-6) + 4*a(n-9) + a(n-11) - a(n-12).
def a(n)
  ary = [1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1]
  (12..n).each{|i|
    ary << 4*ary[-3] - 6*ary[-6] + 4*ary[-9] + ary[-11] - ary[-12]
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