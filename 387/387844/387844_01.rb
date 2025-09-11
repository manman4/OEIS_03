# a(n) = 5*a(n-1) - 10*a(n-2) + 20*a(n-3) + 15*a(n-4) + 4*a(n-5) for n > 4.
def A(n)
  ary = [1,1,1,7,57]
  (4..n).each{|i|
    ary << 5 * ary[-1] - 10 * ary[-2] + 20 * ary[-3] + 15 * ary[-4] + 4 * ary[-5]
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
