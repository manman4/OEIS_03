def A(k, n)
  a, b = 1, 2
  ary = [1]
  cnt = 0
  while cnt < n
    a, b = b, (2 * b + k * k * cnt * a) / (cnt + 2)
    ary << a
    cnt += 1
  end
  ary
end

n = 500
ary = A(16, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
