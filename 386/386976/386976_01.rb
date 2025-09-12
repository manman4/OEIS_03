# a(n) = 4*a(n-1) + 24*a(n-2) + 64*a(n-3) for n > 2.
def A(n)
  ary = [1,6,72]
  (3..n).each{|i|
    ary << 4 * ary[-1] + 24 * ary[-2] + 64 * ary[-3]
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
