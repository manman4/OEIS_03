def A(n)
  ary = [1, 5, 33]
  i = 2
  while i < n
    i += 1
    ary << ((24 - 12 * i) * ary[-3] + (32 * i - 10) * ary[-2] + (9 * i - 9) * ary[-1]) / (i + 1)
  end
  ary
end

n = 5000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}