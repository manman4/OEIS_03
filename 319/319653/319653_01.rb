(0..3124).each{|i| 
  print i
  print ' '
  puts i.to_s(5).split('').sort.join.to_i(5)
}
