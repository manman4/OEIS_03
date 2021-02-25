def A307972(n)
  ary = [1, 1, 1, 1, 1]
  (5..n).each{|i|
    ary << (0..i - 5).inject(0){|s, j| s + ary[j] * ary[i - 5 - j]}
  }
  ary[0..n]
end

n = 6000
ary = A307972(n)
(0..5000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
