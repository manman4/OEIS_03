# a(n) = 3*a(n-1) - a(n-2) + 7*a(n-3) - a(n-4).
def a(n)
  ary = [1, 1, 2, 11]
  (4..n).each{|i|
    ary << 3*ary[-1] - ary[-2] + 7*ary[-3] - ary[-4]
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