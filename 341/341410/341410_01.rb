def A(k, n)
  (1..k).inject(0){|s, i| s + i ** n} % n
end

n = 10000
(1..n).each{|i|
  j = A(4, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
