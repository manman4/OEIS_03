n = 10000
(0..n).each{|i|
  j = 32 * i ** 3 + 48 * i ** 2 + 18 * i + 1
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
