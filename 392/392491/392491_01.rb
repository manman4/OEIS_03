# a(n) = 2*a(n-4) - a(n-8) + a(n-9).
def a(n)
  ary = [1, 0, 0, 0, 2, 0, 0, 0, 3]
  (9..n).each{|i|
    ary << 2*ary[-4] - ary[-8] + ary[-9]
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