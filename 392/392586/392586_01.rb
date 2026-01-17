# a(n) = 10*a(n-1) - 45*a(n-2) + 122*a(n-3) - 220*a(n-4) + 272*a(n-5) - 231*a(n-6) + 130*a(n-7) - 47*a(n-8) + 10*a(n-9) - a(n-10).
def a(n)
  ary = [1, 10, 55, 222, 745, 2242, 6368, 17620, 48196, 130764]
  (10..n).each{|i|
    ary << 10*ary[-1] - 45*ary[-2] + 122*ary[-3] - 220*ary[-4] + 272*ary[-5] - 231*ary[-6] + 130*ary[-7] - 47*ary[-8] + 10*ary[-9] - ary[-10]
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