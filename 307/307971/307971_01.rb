def A307971(n)
  ary = [1, 1, 1, 1]
  (4..n).each{|i|
    ary << (0..i - 4).inject(0){|s, j| s + ary[j] * ary[i - 4 - j]}
  }
  ary[0..n]
end

m = 5000
ary = A307971(m)
(0..4000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
