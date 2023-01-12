def A(n)
 (0..n).inject(0){|s, i| s + i ** (i * (n - i + 1))}
end

n = 10000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}