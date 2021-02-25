def A307395(n)
  a, b, c = 1, 4, 10
  ary = [1]
  while ary.size <= n
    a, b, c = b, c, 4 * c - 6 * b + 3 * a
    ary << a
  end
  ary
end

n = 4000
ary = A307395(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
