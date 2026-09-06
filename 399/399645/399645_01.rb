def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = ((n+1)/2) * (binomial(7*n+6,n) + Sum_{k=0..n} 6^(n-k) * binomial(7*n+7,k)).
def A(n)
  (0..n).inject(ncr(7 * n + 6, n)){|sum, k| sum + 6**(n-k) * ncr(7*n+7, k) } * (n + 1) / 2
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
