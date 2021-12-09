def A(n)
  (0..n).inject(0){|s, i| s + i ** (2 * i)}
end

n = 500
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
