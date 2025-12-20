def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} (k+1) * 2^k * 3^(n-k) * binomial(2*k,2*(n-k)).
def a(n)
  (0..n).inject(0){|s, k| s + (k+1) * 2**k * 3**(n - k) * ncr(2 * k, 2 * (n - k))}
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 