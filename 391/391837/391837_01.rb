# a(n) = 4*a(n-1) - 6*a(n-2) + 12*a(n-3) - 9*a(n-4) - 8*a(n-5) - 16*a(n-6) - 16*a(n-7) - 24*a(n-8) + 32*a(n-9) + 32*a(n-10) - 16*a(n-12).
def a(n)
  ary = [1, 3, 6, 16, 55, 161, 412, 1086, 3013, 8199, 21602, 56748]
  (12..n).each{|i|
    ary << 4*ary[-1] - 6*ary[-2] + 12*ary[-3] - 9*ary[-4] - 8*ary[-5] - 16*ary[-6] - 16*ary[-7] - 24*ary[-8] + 32*ary[-9] + 32*ary[-10] - 16*ary[-12]
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