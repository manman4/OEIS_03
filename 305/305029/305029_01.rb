def A(n)
  a = [0, 2, 2, 2, 2, 0, -2, -2, -2, -2]
  ary = [1] + (1..n).map{|i| a[i % 10]}
end

n = 10000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
