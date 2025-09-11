# a(n) = 4*a(n-3) + 4*a(n-4) - 4*a(n-6) + 8*a(n-7) - 4*a(n-8).
def A(n)
  ary = [1,0,0,2,2,0,4,24]
  (8..n).each{|i|
    ary << 4 * ary[-3] + 4 * ary[-4] - 4 * ary[-6] + 8 * ary[-7] - 4 * ary[-8]
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
