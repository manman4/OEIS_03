# a(n) = 5*a(n-1) - 10*a(n-2) + 10*a(n-3) - 5*a(n-4) + a(n-5) + a(n-6).
def a(n)
  ary = [1, 5, 15, 35, 70, 126]
  (6..n).each{|i|
    ary << 5*ary[-1] - 10*ary[-2] + 10*ary[-3] - 5*ary[-4] + ary[-5] + ary[-6]
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