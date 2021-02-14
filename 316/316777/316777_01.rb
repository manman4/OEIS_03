m = 5
n = 150
s = 1
sum = 0
(1..n).each{|i|
  s *= i
  sum += s ** m
  break if sum.to_s.size > 1000
  print i
  print ' '
  puts sum
}
