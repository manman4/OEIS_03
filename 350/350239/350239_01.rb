def A(n)
  (1..n / 2).inject(0){|s, i| s + (-1) ** (i + 1) * (n ** i / (2 * i) ** i)}
end

n = 5000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}

