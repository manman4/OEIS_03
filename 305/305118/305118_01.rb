def A(n)
  ary = [1]
  (1..n).each{|i|
    ary << (0..i - 1).inject(0){|s, j| s + 1 + ary[j] * ary[-(j + 1)]}
  }
  ary
end
n = 100
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
