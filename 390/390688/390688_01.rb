# a(n) = 2*a(n-5) + a(n-9) - a(n-10).
def a(n)
  ary = [1, 0, 0, 0, 0, 2, 0, 0, 0, 1]
  (10..n).each{|i|
    ary << 2*ary[-5] + ary[-9] - ary[-10]
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