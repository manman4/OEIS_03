def A307970(n)
  ary = [1, 1, 1]
  (3..n).each{|i|
    ary << (0..i - 3).inject(0){|s, j| s + ary[j] * ary[i - 3 - j]}
  }
  ary[0..n]
end

m = 3500
ary = A307970(m)
(0..3000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
