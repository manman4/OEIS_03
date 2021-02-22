n = 1000
(0..500).each{|i|
  print i
  print ' '
  puts (25 * 100 ** i - 20 * 10 ** i + 4) / 9
}
