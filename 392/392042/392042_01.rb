# a(n) = 6*a(n-3) - 12*a(n-6) + 8*a(n-9) + 24*a(n-10).
def a(n)
  ary = [1, 0, 0, 2, 0, 0, 4, 0, 0, 8]
  (10..n).each{|i|
    ary << 6*ary[-3] - 12*ary[-6] + 8*ary[-9] + 24*ary[-10]
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