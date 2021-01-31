(0..7775).each{|i| 
  print i
  print ' '
  puts i.to_s(6).split('').sort.join.to_i(6)
}
