def A(n)
  s = 0
  (1..n).each{|i|
    s += n / n.gcd(i - 1) if n.gcd(i) == 1
  }
  s
end

n = 10000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

