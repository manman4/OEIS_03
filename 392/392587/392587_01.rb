# a(n) = 6*a(n-1) - 15*a(n-2) + 20*a(n-3) - 13*a(n-4) + 5*a(n-6) - 2*a(n-7) - a(n-8).
def a(n)
  ary = [1, 6, 21, 56, 128, 270, 552, 1122]
  (8..n).each{|i|
    ary << 6*ary[-1] - 15*ary[-2] + 20*ary[-3] - 13*ary[-4] + 5*ary[-6] - 2*ary[-7] - ary[-8]
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