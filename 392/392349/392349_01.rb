# a(n) = a(n-1) + 3*a(n-3) + a(n-8).
def a(n)
  ary = [1, 1, 1, 3, 6, 9, 18, 36]
  (8..n).each{|i|
    ary << ary[-1] + 3*ary[-3] + ary[-8]
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