def s(n)
  s = 0
  (1..n).each{|i| s += (2 * i) ** i if n % i == 0}
  s / 2
end

n = 500
(1..n).each{|i|
  j = s(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
