def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..floor(n/2)} binomial(k+2,2) * binomial(n,k) * binomial(n-k,k).
def a(n)
  (0..n / 2).inject(0){|s, k| s + ncr(k+2, 2) * ncr(n, k) * ncr(n-k, k)}
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 