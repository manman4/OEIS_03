# a(n) = 4*a(n-2) - 6*a(n-4) + 4*a(n-6) - a(n-8) + a(n-9).
def a(n)
  ary = [1, 0, 2, 0, 3, 0, 4, 0, 5, 1]
  (9..n).each{|i|
    ary << 4*ary[-2] - 6*ary[-4] + 4*ary[-6] - ary[-8] + ary[-9]
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