# a(n) = a(n-1) + a(n-2) - 2*a(n-5) + a(n-8) + a(n-9) - a(n-10) for n > 15.
def a(n)
  ary = [0, 0, 0, 0, 0, 5, 4, 7, 9, 13, 15, 21, 24, 31, 36, 44]
  (16..n).each{|i|
    ary << ary[-1] + ary[-2] - 2*ary[-5] + ary[-8] + ary[-9] - ary[-10]
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