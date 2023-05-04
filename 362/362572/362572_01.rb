def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  return 1 if n == 0
  m = f(n)
  x = f(k)
  (0..n / k).inject(0){|s, i| s + ((n - (k - 1) * i) / x.to_r) ** i * ncr(n - (k - 1) * i - 1, i) * m / f(n - (k - 1) * i)}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(2, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
