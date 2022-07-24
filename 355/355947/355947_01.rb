def A(n)
  (1..n).inject(0){|s, i| s + (n + 1 - i) * (n / i)}
end

n = 10000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}