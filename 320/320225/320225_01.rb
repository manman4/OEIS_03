def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    ary << (1..i - 1).inject(0){|s, j| s + ary[j] * (i / j - 1)}
  }
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
