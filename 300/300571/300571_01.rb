def A300571(n)
  a = '0'
  [0] + (1..n).map{|i| a = (i.to_s(2) + a.to_s).to_i}
end
n = 200
ary = A300571(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
