def A(k, n)
  (0..n).inject(0){|s, i| s + i ** (k * (n - i))}
end

n = 18
(0..n).each{|i|
  j = A(3, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

