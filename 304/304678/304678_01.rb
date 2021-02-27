require 'prime'

n = 50000
ary = [1]
(2..n).each{|i|
  p_ary = i.prime_division.map{|i| i[1]}
  ary << i if p_ary == p_ary.sort
}
(1..10000).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
