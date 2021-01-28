def A(n)
  a, b, c = 0, 6, 168
  ary = [0]
  while ary.size <= n
    a, b, c = b, c, 27 * c - 27 * b + a
    ary << a
  end
  ary
end

n = 500
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
