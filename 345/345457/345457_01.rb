def A(n)
  ary = [1]
  a, b, c = 1, 57, 1574
  cnt = 0
  while cnt < n
    a, b, c = b, c, 21 * c + 353 * b - 32 * a
    ary << a
    cnt += 1
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