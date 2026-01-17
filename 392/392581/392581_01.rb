# a(n) = 6*a(n-1) - 13*a(n-2) + 14*a(n-3) - 10*a(n-4) + 4*a(n-5) - a(n-6).
def a(n)
  ary = [1, 6, 23, 74, 219, 618]
  (6..n).each{|i|
    ary << 6*ary[-1] - 13*ary[-2] + 14*ary[-3] - 10*ary[-4] + 4*ary[-5] - ary[-6]
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