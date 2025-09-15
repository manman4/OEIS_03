n = 1000
(0..n).each{|i|
  j = (1 + 8*64 ** i + 9*(-27) ** i)/18
  break if j.to_s.length > 1000
  print i
  print ' '
  puts j
}
