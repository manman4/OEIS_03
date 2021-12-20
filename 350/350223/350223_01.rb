def A(n)
  (1..n).inject(0){|s, i| s + (-1) ** (i + 1) * (n ** i / i ** i)}
end

n = 5000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}
