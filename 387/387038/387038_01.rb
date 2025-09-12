# a(n) = 5*a(n-1) + 5*a(n-2) + 145*a(n-3) + 55*a(n-4) + 8*a(n-5) for n > 4.
def A(n)
  ary = [1,2,57,474,3004]
  (5..n).each{|i|
    ary << 5 * ary[-1] + 5 * ary[-2] + 145 * ary[-3] + 55 * ary[-4] + 8 * ary[-5]
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
