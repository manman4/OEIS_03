def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = ((n+1)/2) * (binomial(3*n+2,n) + Sum_{k=0..n} 2^(n-k) * binomial(3*n+3,k)).
def A(n)
  (0..n).inject(ncr(3*n+2, n)){|sum, k| sum + 2**(n-k) * ncr(3*n+3, k) } * (n + 1) / 2
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
