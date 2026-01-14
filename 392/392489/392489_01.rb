# a(n) = 2*a(n-2) - a(n-4) + a(n-5).
def a(n)
  ary = [1, 0, 2, 0, 3]
  (5..n).each{|i|
    ary << 2*ary[-2] - ary[-4] + ary[-5]
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