n = 10000
(0..n).each{|i|
  print i
  print ' '
  puts (3 * i * i - 1) * (3 * i * i - 2) * (3 * i * i * i - 3 * i + 1) / 2
}