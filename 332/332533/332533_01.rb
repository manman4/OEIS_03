def A(n)
  ary = [0] + (1..n).map{|i| (1..i).inject(0){|s, j| s + (i / j) * i ** (j - 1)}}
end

n = 500
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}