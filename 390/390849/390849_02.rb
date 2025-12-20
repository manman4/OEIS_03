# a(n) = 8*a(n-1) - 16*a(n-3) - 328*a(n-4) - 96*a(n-5) + 1728*a(n-7) - 1296*a(n-8).
def a(n)
  ary = [1, 4, 24, 248, 1628, 11232, 77632, 513088]
  (8..n).each{|i|
    ary << 8*ary[-1] - 16*ary[-3] - 328*ary[-4] - 96*ary[-5] + 1728*ary[-7] - 1296*ary[-8]
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