def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} 3^(n-k) * binomial(n,k) * binomial(n+2,k).
def A(n)
  (0..n).inject(0){|sum, i| sum + 3**(n - i) * ncr(n, i) * ncr(n + 2, i)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.size > 1000
  print i
  print ' '
  puts j
}
