def A(k, n)
  (0..n).inject(0){|s, i| s + i ** (k * n - (k - 1) * i)}
end

n = 17
(0..n).each{|i|
  j = A(2, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

