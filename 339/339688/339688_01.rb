def A(k, n)
  s = 0
  (1..n).each{|i| s += k ** (i - 1) if n % i == 0}
  s
end

n = 1000
(1..n).each{|i|
  j = A(8, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
