(0..6560).each{|i| 
  print i
  print ' '
  puts i.to_s(3).split('').sort.reverse.join.to_i(3)
}
