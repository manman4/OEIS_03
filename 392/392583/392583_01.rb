# a(n) = 10*a(n-1) - 43*a(n-2) + 110*a(n-3) - 191*a(n-4) + 232*a(n-5) - 200*a(n-6) + 118*a(n-7) - 45*a(n-8) + 10*a(n-9) - a(n-10).
def a(n)
  ary = [1, 10, 57, 250, 958, 3422, 11759, 39416, 129700, 420542]
  (10..n).each{|i|
    ary << 10*ary[-1] - 43*ary[-2] + 110*ary[-3] - 191*ary[-4] + 232*ary[-5] - 200*ary[-6] + 118*ary[-7] - 45*ary[-8] + 10*ary[-9] - ary[-10]
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