def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A(n)
  (1..n).inject(0){|s, i| s + phi(i / (i.gcd(n)))}
end

n = 1000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

  