def A(n)
  (1..n).inject(0){|s, i| s + (i.gcd(n)) ** (i.gcd(n))}
end

n = 400
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}