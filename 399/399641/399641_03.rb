def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} 2^k * binomial(k+2,2) * binomial(3*n+4,n-k).
def A(n)
  (0..n).inject(0){|sum, k| sum + 2**k * ncr(k+2, 2) * ncr(3*n+4, n-k) }
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
