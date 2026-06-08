(0..10000).each{|i|
  j = i**4 + 14*i**3 + 63*i**2 + 98*i + 28
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
