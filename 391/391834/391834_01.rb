# a(n) = 8*a(n-1) - 24*a(n-2) + 40*a(n-3) - 32*a(n-4) - 32*a(n-5) + 40*a(n-6) - 32*a(n-7) - 96*a(n-8) + 32*a(n-9) + 64*a(n-10) - 16*a(n-12).
def a(n)
  ary = [1, 6, 24, 86, 320, 1232, 4696, 17488, 64160, 233872, 848960, 3067200]
  (12..n).each{|i|
    ary << 8*ary[-1] - 24*ary[-2] + 40*ary[-3] - 32*ary[-4] - 32*ary[-5] + 40*ary[-6] - 32*ary[-7] - 96*ary[-8] + 32*ary[-9] + 64*ary[-10] - 16*ary[-12]
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