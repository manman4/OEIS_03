(0..4095).each{|i| 
  print i
  print ' '
  puts i.to_s(8).split('').sort.join.to_i(8)
}
