n = 5000
a, b = 1, 1
(1..n).each{|i|
  print i
  print ' '
  puts a
  c = a.gcd(b)
  a, b = b, c + (a + b) / c
  break if a.to_s.size > 1000
}