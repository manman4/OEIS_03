def A(k, n)
  s = 1
  (0..n).each{|i|
    s *= i ** k + (n - i) ** k
  }
  s
end

n = 300
(0..n).each{|i|
  j = A(2, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
