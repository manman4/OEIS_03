def A(k, n)
  ary = [1]
  a, b, c, d = 1, 1, 1, 1
  while ary.size <= n
    break if (b * d + k * c * c) % a > 0
    a, b, c, d = b, c, d, (b * d + k * c * c) / a
    ary << a
  end
  ary
end

n = 200
ary = A(-2, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
