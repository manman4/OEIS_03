n = 10000
(0..n).each{|i|
  j = 2 ** (i.to_s.split('').map(&:to_i).inject(:+))
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
