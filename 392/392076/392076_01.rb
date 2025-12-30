# a(n) = 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 70*a(n-4) + 58*a(n-5) - 36*a(n-6) + 20*a(n-7) - 9*a(n-8) + 2*a(n-9) - a(n-10).
def a(n)
  ary = [1, 2, 3, 4, 5, 11, 37, 113, 289, 640]
  (10..n).each{|i|
    ary << 8*ary[-1] - 28*ary[-2] + 56*ary[-3] - 70*ary[-4] + 58*ary[-5] - 36*ary[-6] + 20*ary[-7] - 9*ary[-8] + 2*ary[-9] - ary[-10]
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