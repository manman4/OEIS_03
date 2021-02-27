def A(s, t, n)
  a, b = 1, s
  ary = [1]
  cnt = 0
  while cnt < n
    a, b = b, s * b * (cnt + 2) + t * a
    ary << a
    cnt += 1
  end
  ary
end

n = 1000
ary = A(3, -2, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
