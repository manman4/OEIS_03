# a(n) = 8*a(n-2) + 8*a(n-3) - 24*a(n-4) - 16*a(n-5) + 8*a(n-6) - 32*a(n-7) - 48*a(n-8) + 96*a(n-9) - 96*a(n-10) + 64*a(n-11) - 16*a(n-12).
def a(n)
  ary = [1, 0, 6, 6, 24, 80, 104, 560, 800, 2960, 6720, 15200]
  (12..n).each{|i|
    ary << 8*ary[-2] + 8*ary[-3] - 24*ary[-4] - 16*ary[-5] + 8*ary[-6] - 32*ary[-7] - 48*ary[-8] + 96*ary[-9] - 96*ary[-10] + 64*ary[-11] - 16*ary[-12]
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