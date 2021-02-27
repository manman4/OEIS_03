def A(n)
  a = 1
  (1..n).each{|i|
    a *= i ** 2 + (n - i + 1) ** 2
  }
  a
end

n = 200
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
