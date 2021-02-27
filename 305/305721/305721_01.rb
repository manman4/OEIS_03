def A305721(n)
  a, b, c, d, e, f, g, h = 1, 99, 1765, 14407, 74313, 284075, 880685, 2340495
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h = b, c, d, e, f, g, h, 8*h - 28*g + 56*f - 70*e + 56*d - 28*c + 8*b - a
    ary << a
  end
  ary
end
n = 10000
ary = A305721(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
