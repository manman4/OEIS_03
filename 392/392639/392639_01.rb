# a(n) = 10*a(n-1) - 45*a(n-2) + 120*a(n-3) - 210*a(n-4) + 254*a(n-5) - 220*a(n-6) + 140*a(n-7) - 65*a(n-8) + 20*a(n-9) - 4*a(n-10).
def a(n)
  ary = [1, 10, 55, 220, 715, 2004, 5035, 11680, 25670, 54740]
  (10..n).each{|i|
    ary << 10*ary[-1] - 45*ary[-2] + 120*ary[-3] - 210*ary[-4] + 254*ary[-5] - 220*ary[-6] + 140*ary[-7] - 65*ary[-8] + 20*ary[-9] - 4*ary[-10]
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