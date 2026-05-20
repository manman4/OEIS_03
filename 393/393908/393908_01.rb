# a(n) = 13*a(n-1) - 63*a(n-2) + 154*a(n-3) - 215*a(n-4) + 189*a(n-5) - 108*a(n-6) + 40*a(n-7) - 9*a(n-8) + a(n-9).
def a(n)
  ary = [1, 3, 15, 82, 459, 2589, 14632, 82708, 467399]
  (9..n).each{|i|
    ary << 13 * ary[-1] - 63 * ary[-2] + 154 * ary[-3] - 215 * ary[-4] + 189 * ary[-5] - 108 * ary[-6] + 40 * ary[-7] - 9 * ary[-8] + ary[-9]
  }
  ary
end

n = 500
ary = a(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}