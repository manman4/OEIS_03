def A(k, n)
  s = 0
  (1..n).each{|i|
    s += i ** k if n.gcd(i) == 1
  }
  s
end

n = 1000
(1..n).each{|i|
  j = A(i, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
