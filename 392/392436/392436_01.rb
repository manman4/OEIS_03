# a(n) = 6*a(n-4) + 2*a(n-5) + a(n-7) - 3*a(n-8) + 3*a(n-9) - a(n-10).
def a(n)
  ary = [1, 0, 0, 0, 3, 1, 0, 1, 15, 15]
  (10..n).each{|i|
    ary << 6*ary[-4] + 2*ary[-5] + ary[-7] - 3*ary[-8] + 3*ary[-9] - ary[-10]
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