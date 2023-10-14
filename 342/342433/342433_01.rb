def A(k, n)
  (1..n).inject(0){|s, i| s + (i.gcd(n)) ** k}
end

n = 500
(1..n).each{|i|
  # i=1もOK
  j = A(i - 1, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}