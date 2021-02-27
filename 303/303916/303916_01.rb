n = 10000
ary = (0..n).map{|i| (i - 1) * i * (i + 1) * (i + 2) * (2 * i + 1) / 10}
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
