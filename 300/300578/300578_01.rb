def A300558(n)
  a = '1'
  [1] + (2..n).map{|i| a = (a.to_s + i.to_s(2) * i).to_i}
end
n = 50
ary = A300558(n).map{|i| i.to_s.to_i(2)}
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
