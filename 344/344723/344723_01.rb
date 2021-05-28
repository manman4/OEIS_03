def A(n)
  ary = [0] + (1..n).map{|i| (1..i).inject(0){|s, j| s + (-1) ** ((j + 1) % 2) * (i / j) ** 5}}
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}