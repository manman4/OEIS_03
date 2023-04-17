def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n, k)
  m = f(n)
  (0..n / k).inject(0){|s, i| s + (-n) ** i * m / (f(i) * f(n - k * i))}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(i, 5)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
