def s(k, n)
  s = 0
  (1..n).each{|i| s += i ** (k * n) if n % i == 0}
  s
end

n = 30
(1..n).each{|i|
  j = s(i, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
