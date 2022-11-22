def A(n)
  ary = [1, 1, 2, 6, 25]
  (5..n).each{|i|
    ary << i * ary[-1] + ary[-4] - i * ary[-5]
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
