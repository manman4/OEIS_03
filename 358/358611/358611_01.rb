def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  (0..n / k).inject(0){|s, i| s + (-1) ** i * f(n - k * i)}
end

n = 1000
(0..n).each{|i|
  j = A(5, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
