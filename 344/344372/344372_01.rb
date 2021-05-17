def A(n)
  (1..2 * n).inject(0){|s, i| s + (-1) ** (i % 2) * i.gcd(2 * n)}
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}