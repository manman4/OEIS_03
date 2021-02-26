def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + f(k * n) / (f(n - i) ** k * f(i) ** k)}
end

n = 30
(0..n).each{|i|
  j = A(i, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
