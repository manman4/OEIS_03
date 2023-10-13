def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A(k, n)
  (1..n).inject(0){|s, i| s + phi(i) * (n / i) ** k}
end

n = 1000
(1..n).each{|i|
  j = A(i, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
