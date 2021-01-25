def A(n)
  ary = [3]
  (2..n).each{|i|
    j = 10 ** (2 ** (i - 2))
    ary << (j + 3) * (j - 1) / 3 - ary[-1]
  }
  ary
end

n = 15
ary = A(n)
(1..n).each{|i|
  j = ary[i - 1] / 3
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
