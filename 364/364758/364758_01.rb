def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = (1/n) * Sum_{k=0..n-1} (-1)^k * binomial(n,k) * binomial(4*n-3*k,n-1-k) for n > 0.
def A(n)
  return 1 if n == 0
  (0..n - 1).inject(0){|s, i| s + (-1) ** i * ncr(n, i) * ncr(4 * n - 3 * i, n - 1 - i)} / n
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
