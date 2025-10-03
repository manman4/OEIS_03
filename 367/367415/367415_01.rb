def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} binomial(n+k,k) * binomial(2*n,n-4*k).
def A(n)
  (0..n / 4).inject(0){|sum, k| sum + ncr(n + k, k) * ncr(2 * n, n - 4 * k)} / (n + 1)
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
