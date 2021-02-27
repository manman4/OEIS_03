def A(k, m, n)
  a, b = 0, m
  ary = [0]
  cnt = 0
  while cnt < n
    a, b = b, (2 * m * b + k * k * cnt * a) / (cnt + 2r)
    a = a.to_i if a.denominator == 1
    ary << a
    cnt += 1
  end
  ary
end

n = 1000
ary = A(4, 1, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
