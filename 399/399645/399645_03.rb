def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} 6^k * binomial(k+2,2) * binomial(7*n+8,n-k).
def A(n)
  (0..n).inject(0){|sum, k| sum + 6**k * ncr(k+2, 2) * ncr(7*n+8, n-k) }
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
