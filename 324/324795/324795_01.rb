require 'prime'

p_ary = [0] + Prime.take(10 ** 6).to_a

(1..10000).each{|i|
  j = 2 * p_ary[i] * p_ary[i + 2] - p_ary[i + 1] ** 2
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
