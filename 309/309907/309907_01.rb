n = 1000
(0..500).each{|i|
  print i
  print ' '
  puts (16 * 100 ** i - 8 * 10 ** i + 1) / 9
}
