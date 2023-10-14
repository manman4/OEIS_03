def A(k, n)
  ary = [1]
  (1..n).each{|i|
    ary << ary[-1] - (0..i - k).inject(0){|s, j| s + ary[j] * ary[-j - k]}
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
