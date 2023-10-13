n = 1000
(0..n).each{|i|
  j = (0..i).inject(0){|s, j| s + 2 ** j * j ** (i - j)}
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}