def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end
def A(n)
  (0..n).inject(0){|s, i| s + (-3) ** i * f(3 * n - 2 * i) / (f(n - i) ** 3 * f(i))}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
