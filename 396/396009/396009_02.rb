# a(n) = a(n-1) + a(n-2) - a(n-4) - a(n-5) + a(n-6) for n > 14.
def a(n)
  ary = [0, 0, 0, 0, 0, 1, 2, 4, 7, 7, 9, 11, 13, 15, 18]
  (15..n).each{|i|
    ary << (ary[-1] + ary[-2] - ary[-4] - ary[-5] + ary[-6])
  }
  ary
end

n = 10000
ary = a(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}