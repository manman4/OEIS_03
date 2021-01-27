def A(k, n)
  a, b = 1, 2 * k
  ary = [1]
  cnt = 0
  while cnt + 1 <= n
    a, b = b, 2 * k * b - a
    cnt += 1
    ary << a
  end
  ary
end

n = 400
(0..n).each{|i|
  j = A(i, i)[-1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
