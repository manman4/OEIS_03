# a(n) = 3*a(n-3) + 3*a(n-4) - 3*a(n-6) + 21*a(n-7) - 3*a(n-8) + a(n-9) + 3*a(n-10) + 3*a(n-11) + a(n-12).
def a(n)
  ary = [1, 0, 0, 1, 1, 0, 1, 20, 1, 1, 84, 84]
  (12..n).each{|i|
    ary << 3*ary[-3] + 3*ary[-4] - 3*ary[-6] + 21*ary[-7] - 3*ary[-8] + ary[-9] + 3*ary[-10] + 3*ary[-11] + ary[-12]
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