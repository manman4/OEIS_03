def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = (4/n) * Sum_{k=0..floor(n/2)} binomial(k+1,2) * binomial(2*n,n-2*k) for n > 0.
def a(n) 
  return 1 if n == 0
  (0..n / 2).inject(0){|s, k| s + ncr(k + 1, 2) * ncr(2 * n, n - 2 * k)} * 4 / n
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 