def A(k, n)
  ary = [1] * k
  (k..n).each{|i|
    ary << ((k - 1) * ary[-1] - i * ary[-k] + 1) / k
  }
  ary
end

n = 1000
ary = A(4, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
