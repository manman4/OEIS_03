def A(n)
  ary = [1] * 3
  (3..n).each{|i|
    ary << (3 * (2 * i - 1) * ary[-1] - i * ary[-2] + 2 * (i - 1) * i * (2 * i - 3) * ary[-3] + 2 * (2 * i - 3)) / (9 * (i - 1)) 
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
