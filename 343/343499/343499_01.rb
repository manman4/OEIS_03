def A(k, n)
  (1..n).inject(0){|s, i| s + (i.gcd(n)) ** k}
end

n = 10000
(1..n).each{|i|
  j = A(5, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
