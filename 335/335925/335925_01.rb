def A(n)
  ary = [0, 1]
  (2..n).each{|i| ary << ary[(i - 1) / ary[i - 1]] + 1}
  ary
end

m = 10100
n = 10000
ary = A(m)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
