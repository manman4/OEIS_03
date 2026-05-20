# a(n) = 15*a(n-1) - 83*a(n-2) + 220*a(n-3) - 303*a(n-4) + 220*a(n-5) - 83*a(n-6) + 15*a(n-7) - a(n-8).
def a(n)
  ary = [1, 4, 22, 130, 789, 4848, 29975, 185953]
  (8..n).each{|i|
    ary << 15*ary[-1] - 83*ary[-2] + 220*ary[-3] - 303*ary[-4] + 220*ary[-5] - 83*ary[-6] + 15*ary[-7] - ary[-8]
  }
  ary
end

n = 200
ary = a(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}