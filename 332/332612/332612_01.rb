def A(n)
  s = 0
  (2..n - 1).each{|i|
    (1..i - 1).each{|j|
      s += (n - i) * (n - j) if i.gcd(j) == 1
    }
  }
  s
end

n = 1000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}