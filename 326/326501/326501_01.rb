n = 1000
s = 0
(0..n).each{|i|
  s += (-i) ** i
  break if s.to_s.size > 1000
  print i
  print ' '
  puts s
}
