def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + (-1) ** (n-i) * f(n + (k - 1) * i) / (f(n - i) * f(i) ** k)}.to_i
end

n = 100
(0..n).each{|i|
  j = A(i, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

