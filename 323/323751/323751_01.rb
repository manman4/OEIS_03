def A(n)
  s = 1
  (0..n).each{|i|
    s *= i ** n + (n - i) ** n
  }
  s
end

n = 50
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
