# a(n) = 13*a(n-1) - 63*a(n-2) + 154*a(n-3) - 215*a(n-4) + 189*a(n-5) - 108*a(n-6) + 40*a(n-7) - 9*a(n-8) + a(n-9) for n > 9.
def a(n)
  ary = [1, 1, 7, 43, 252, 1445, 8209, 46461, 262587, 1483354]
  (10..n).each{|i|
    ary << 13*ary[-1] - 63*ary[-2] + 154*ary[-3] - 215*ary[-4] + 189*ary[-5] - 108*ary[-6] + 40*ary[-7] - 9*ary[-8] + ary[-9]
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