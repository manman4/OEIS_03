def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = n! * Sum_{k=0..n} (2*n+k+1)^(k-1) * binomial(2*n+k+1,n-k)/k!.
def A(n)
  m = f(n)
  (0..n).inject(0){|s, k| s + (2 * n + k + 1) ** (k - 1) * ncr(2 * n + k + 1, n - k) * m / f(k)}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
