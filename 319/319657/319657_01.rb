(0..6560).each{|i| 
  print i
  print ' '
  puts i.to_s(9).split('').sort.join.to_i(9)
}
