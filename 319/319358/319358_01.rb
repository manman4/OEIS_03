n = 100
(0..n).each{|i|
  j = (10 ** i - 1) ** 9
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
