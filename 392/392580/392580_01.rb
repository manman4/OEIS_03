# a(n) = 12*a(n-1) - 56*a(n-2) + 140*a(n-3) - 230*a(n-4) + 262*a(n-5) - 212*a(n-6) + 120*a(n-7) - 45*a(n-8) + 10*a(n-9) - a(n-10).
def a(n)
  ary = [1, 12, 88, 524, 2810, 14198, 69068, 327240, 1520519, 6959930]
  (10..n).each{|i|
    ary << 12*ary[-1] - 56*ary[-2] + 140*ary[-3] - 230*ary[-4] + 262*ary[-5] - 212*ary[-6] + 120*ary[-7] - 45*ary[-8] + 10*ary[-9] - ary[-10]
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