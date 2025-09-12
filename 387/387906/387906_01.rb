# a(n) = 5*a(n-1) + 130*a(n-2) + 740*a(n-3) - 65*a(n-4) + 16*a(n-5) for n > 4.
def A(n)
  ary = [1,22,474,6008,107883]
  (5..n).each{|i|
    ary << 5 * ary[-1] + 130 * ary[-2] + 740 * ary[-3] - 65 * ary[-4] + 16 * ary[-5]
  }
  ary
end

n = 1100
m = 1000
ary = A(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
