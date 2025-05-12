n = 1000
(0..n).each{|i|
  # a(n) = (2*16^(n+3) - 9^(n+4) + 14*4^(n+3) - 7)/2520.
  j = (2 * 16**(i + 3) - 9**(i + 4) + 14 * 4**(i + 3) - 7) / 2520
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
