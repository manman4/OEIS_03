# a(n) = a(n-1) + 3*a(n-5) + a(n-14).
def a(n)
  ary = [1, 1, 1, 1, 1, 3, 6, 9, 12, 15, 24, 42, 69, 105]
  (14..n).each{|i|
    ary << ary[-1] + 3*ary[-5] + ary[-14]
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