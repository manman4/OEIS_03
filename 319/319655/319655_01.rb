(0..2400).each{|i| 
  print i
  print ' '
  puts i.to_s(7).split('').sort.join.to_i(7)
}
