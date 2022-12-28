def A279060(n)
  s = 0
  (1..n).each{|i|
    s += 1 if n % i == 0 && i % 6 == 1
  }
  s
end

n = 10000
(1..n).each{|i| 
  j = A279060(6 * i - 2)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
