def A(n)
  (1..n).inject(0){|s, i| s + (-2) ** (n - n / i)}
end

n = 3000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}