def A001878(n)
  s = 0
  (1..n).each{|i|
    s += 1 if n % i == 0 && i % 5 == 3
  }
  s
end

n = 10000
(1..n).each{|i| 
  j = A001878(5 * i - 3)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
