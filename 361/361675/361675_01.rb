def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n, k)
  m = f(n)
  (0..n / (2 * k - 1)).inject(0){|s, i| s + m / (f(i) ** k * f((k - 1) * i) * f(n - (2 * k - 1) * i))}
end

n = 1000
(0..n).each{|i|
  j = A(i, 4)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}