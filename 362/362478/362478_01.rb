def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  m = f(n)
  (0..n / 3).inject(0){|s, i| s + m * (k / 6r) ** i * (3 * i + 1) ** (n - 2 * i - 1) / (f(i) * f(n - 3 * i))}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(2, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
