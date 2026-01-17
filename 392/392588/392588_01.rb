# a(n) = 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 68*a(n-4) + 48*a(n-5) - 16*a(n-6).
def a(n)
  ary = [1, 8, 36, 120, 332, 816]
  (6..n).each{|i|
    ary << 8*ary[-1] - 28*ary[-2] + 56*ary[-3] - 68*ary[-4] + 48*ary[-5] - 16*ary[-6]
  }
  ary
end

n = 1000
ary = a(n)
(0..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 