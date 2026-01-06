# a(n) = 3*a(n-2) - 3*a(n-4) + a(n-5) + a(n-6).
def a(n)
  ary = [1, 0, 1, 0, 1, 1]
  (6..n).each{|i|
    ary << 3*ary[-2] - 3*ary[-4] + ary[-5] + ary[-6]
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