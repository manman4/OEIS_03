# a(n) = 3*a(n-3) - 3*a(n-6) + a(n-9) + a(n-10).
def a(n)
  ary = [1, 0, 0, 3, 0, 0, 6, 0, 0, 10]
  (10..n).each{|i|
    ary << 3*ary[-3] - 3*ary[-6] + ary[-9] + ary[-10]
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