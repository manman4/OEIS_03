def A(n)
  a, b = 0, 1
  cnt = 0
  ary = [0]
  while cnt < n
    cnt += 1
    a, b = b, 4 * (cnt + 1) * b - (2 * cnt + 1) ** 2 * a
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
