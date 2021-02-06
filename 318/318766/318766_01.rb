require 'prime'

n = 500
ary = [1] + Prime.take(n).map{|i| i * i - 1}
s = 1
(0..n).each{|i|
  s *= ary[i]
  break if s.to_s.size > 1000
  print i
  print ' '
  puts s
}
