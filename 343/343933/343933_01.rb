n = 10000
s = 0
(1..n).each{|i|
  s += (-i) ** i
  print i
  print ' '
  puts s % i
}