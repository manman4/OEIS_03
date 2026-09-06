def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} 4^k * binomial(k+2,2) * binomial(5*n+6,n-k).
def A(n)
  (0..n).inject(0){|sum, k| sum + 4**k * ncr(k+2, 2) * ncr(5*n+6, n-k) }
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
