(0..13).each{|i|
  j = 2 ** (2 ** i - i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
