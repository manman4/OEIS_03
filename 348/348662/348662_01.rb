def A(n)
  ary = [1, -3, 8, -30]
  (4..n).each{|i|
    ary << -4 * (2 * (i - 2) * ary[-1] + (5 * i - 14) * ary[- 2] + 8 * (i - 3) * ary[- 3] + 16 * (i - 4) * ary[- 4]) / (i - 1)
  }
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
