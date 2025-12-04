def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = 3*4^(n-2)/2 + binomial(2*n-2,n) + (n-1)*binomial(2*n-4,n-2)/2 for n > 1.
def a(n)
  return 0**n if n < 2
  (3*4**(n-2) + 2*ncr(2*n-2, n) + (n-1)*ncr(2*n-4, n-2)) / 2
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 