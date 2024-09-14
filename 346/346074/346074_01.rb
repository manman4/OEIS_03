def A(k, n)
  ary = [1] * k
  (k..n).each{|i|
    ary << (0..i - k).inject(1){|s, j| s + ary[j] * ary[-k - j]}
  }
  ary
end

n = 1100
m = 1000
ary = A(5, n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
