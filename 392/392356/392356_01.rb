# a(n) = 5*a(n-2) - 10*a(n-4) + 10*a(n-6) - 5*a(n-8) + a(n-9) + a(n-10).
def a(n)
  ary = [1, 0, 1, 0, 1, 0, 1, 0, 1, 1]
  (10..n).each{|i|
    ary << 5*ary[-2] - 10*ary[-4] + 10*ary[-6] - 5*ary[-8] + ary[-9] + ary[-10]
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