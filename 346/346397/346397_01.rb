def A(k, n)
  ary = [0, 1]
  (2..n).each{|i|
    ary << (i + k - 1) * ary[-1] - k * (i - 1) * ary[-2] + k ** (i - 1)
  }
  ary
end

n = 1000
ary = A(-2, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}