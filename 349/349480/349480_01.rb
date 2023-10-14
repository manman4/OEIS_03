def A(n)
  return 1 if n < 2
  (0..n).inject(0){|s, i| s + (-1) ** ((n - i) % 2) * ((i - 1) * n + 1..i * n).inject(:*)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
