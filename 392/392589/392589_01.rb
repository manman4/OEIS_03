# a(n) = 10*a(n-1) - 45*a(n-2) + 120*a(n-3) - 208*a(n-4) + 242*a(n-5) - 190*a(n-6) + 100*a(n-7) - 36*a(n-8) + 8*a(n-9) - a(n-10).
def a(n)
  ary = [1, 10, 55, 220, 717, 2032, 5245, 12800, 30433, 71936]
  (10..n).each{|i|
    ary << 10*ary[-1] - 45*ary[-2] + 120*ary[-3] - 208*ary[-4] + 242*ary[-5] - 190*ary[-6] + 100*ary[-7] - 36*ary[-8] + 8*ary[-9] - ary[-10]
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