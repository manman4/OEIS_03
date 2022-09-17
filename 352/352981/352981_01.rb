def A(k, n)
  (0..n / k).inject(0){|s, i| s + i ** n}
end

n = 1000
(0..n).each{|i|
  j = A(2, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
