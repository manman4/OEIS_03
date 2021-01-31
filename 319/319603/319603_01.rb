(0..10000).each{|i| 
  print i
  print ' '
  puts (i * i * i).to_s.reverse.to_i + i * i * i
}
