def A000085(n)
  cnt = 0
  a, b = 1, 1
  ary = [1]
  while cnt < n
    cnt += 1
    a, b = b, b + cnt * a
    ary << a
  end
  ary
end

n = 500
ary = A000085(4 * n + 3)
(0..n).each{|i|
  j = ary[4 * i + 3]
  break if j % (2 ** (i + 2)) > 0
  j /= (2 ** (i + 2))
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
