def A300517(n)
  a = '1'
  [1] + (2..n).map{|i| a = (i.to_s * i + a.to_s).to_i}
end
n = 100
ary = A300517(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
