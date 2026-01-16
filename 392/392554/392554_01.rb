# a(n) = 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + 2*a(n-6) - 4*a(n-7) + 2*a(n-8) - a(n-12).
def a(n)
  ary = [1, 4, 10, 20, 35, 56, 86, 132, 207, 332, 538, 868]
  (12..n).each{|i|
    ary << 4*ary[-1] - 6*ary[-2] + 4*ary[-3] - ary[-4] + 2*ary[-6] - 4*ary[-7] + 2*ary[-8] - ary[-12]
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