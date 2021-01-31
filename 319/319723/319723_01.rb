(0..7775).each{|i| 
  print i
  print ' '
  puts i.to_s(6).split('').sort.reverse.join.to_i(6)
}
