def A(n)
  ary = [1, -3, 17]
  i = 2
  while i < n
    i += 1
    ary << ((8 - 4 * i) * ary[-3] + (30 - 24 * i) * ary[-2] + (17 - 37 * i) * ary[-1]) / (3 * i + 3)
  end
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}