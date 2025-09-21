def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} binomial(n+1,k) * binomial(5*n-5*k+5,n-2*k).
def A(n)
  (0..n / 2).inject(0){|sum, i| sum + ncr(n + 1, i) * ncr(5 * n - 5 * i + 5, n - 2 * i)} / (n + 1)
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
