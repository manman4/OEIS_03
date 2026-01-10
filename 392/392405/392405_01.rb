# a(n) = 3*a(n-1) - 3*a(n-2) + a(n-3) + 6*a(n-5) + 2*a(n-6) + a(n-9).
def a(n)
  ary = [1, 1, 1, 1, 1, 5, 21, 57, 121]
  (9..n).each{|i|
    ary << 3*ary[-1] - 3*ary[-2] + ary[-3] + 6*ary[-5] + 2*ary[-6] + ary[-9]
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