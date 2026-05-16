# a(n) = a(n-1) + a(n-2) - 2*a(n-5) + a(n-8) + a(n-9) - a(n-10) for n > 20.
def a(n)
  ary = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,3,4,6]
  (21..n).each{|i|
    ary << (ary[i-1] + ary[i-2] - 2*ary[i-5] + ary[i-8] + ary[i-9] - ary[i-10])
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