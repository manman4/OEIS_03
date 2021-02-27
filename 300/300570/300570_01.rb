def A300570(n)
  a = '1'
  [1] + (2..n).map{|i| a = (i.to_s(2) + a.to_s).to_i}
end
n = 500
ary = A300570(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
