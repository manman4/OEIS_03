def A(n)
  a, b, c = 1, 1, 3
  cnt = 0
  ary = [1]
  while cnt < n
    cnt += 1
    a, b, c = b, c, (3 * (9 * (cnt + 2) ** 2 - 19 * (cnt + 2) + 8) * c - 3 * (cnt + 1) * (3 * cnt + 2) * b - 2 * (2 * cnt + 1) * (3 * cnt + 4) * a) / (2 * (cnt + 2) * (3 * cnt + 1))
    ary << a
  end
  ary
end

n = 2000
ary = A(n)
(0..n).each{|i|
  j = (ary[i] - (-1) ** (i % 2)) / 2
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
