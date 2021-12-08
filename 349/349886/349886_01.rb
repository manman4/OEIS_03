def A(n)
  (0..n).inject(0){|s, i| s + i ** (i * n)}
end

n = 8
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

