require 'prime'

n = 10000
p_ary = [0] + Prime.take(n).to_a
s = 0
(1..n).each{|i|
  s += (2 * i - 1) * p_ary[i]
  j = s
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
