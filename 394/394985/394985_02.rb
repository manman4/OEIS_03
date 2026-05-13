# a(n) = a(n-1) + a(n-2) - a(n-5) - a(n-6) - a(n-7) + a(n-8) + a(n-9) + a(n-10) - a(n-13) - a(n-14) + a(n-15) for n > 21.
def a(n)
  ary = [0, 0, 0, 0, 0, 0, 6, 5, 9, 12, 18, 22, 31, 37, 49, 59, 74, 87, 108, 125, 150, 174]
  (22..n).each{|i|
    ary << ary[-1] + ary[-2] - ary[-5] - ary[-6] - ary[-7] + ary[-8] + ary[-9] + ary[-10] - ary[-13] - ary[-14] + ary[-15]
  }
  ary
end

n = 10050
m = 10000
ary = a(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}