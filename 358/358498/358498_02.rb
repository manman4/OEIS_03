def A(n)
  ary = [1, 1, 2, 7]
  (4..n).each{|i|
    ary << i * ary[-1] + ary[-3] - i * ary[-4]
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
