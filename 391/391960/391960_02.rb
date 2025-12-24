# a(n) = 8*a(n-3) - 24*a(n-6) + 24*a(n-7) + 32*a(n-9) - 96*a(n-10) - 16*a(n-12) + 96*a(n-13) - 144*a(n-14).
def a(n)
  ary = [1, 0, 0, 4, 0, 0, 12, 36, 0, 32, 288, 0, 80, 1440]
  (14..n).each{|i|
    ary << 8*ary[-3] - 24*ary[-6] + 24*ary[-7] + 32*ary[-9] - 96*ary[-10] - 16*ary[-12] + 96*ary[-13] - 144*ary[-14]
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