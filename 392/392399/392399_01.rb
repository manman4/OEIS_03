# a(n) = 2*a(n-1) - a(n-2) + 4*a(n-5) + a(n-9).
def a(n)
  ary = [1, 1, 1, 1, 1, 4, 11, 22, 37]
  (9..n).each{|i|
    ary << 2*ary[-1] - ary[-2] + 4*ary[-5] + ary[-9]
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