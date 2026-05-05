def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} 4^k * binomial(k+3,3) * binomial(5*n+4,n-k).
def a(n)
  (0..n).inject(0){|s, k| s + 4**k * ncr(k + 3, 3) * ncr(5 * n + 4, n - k)}
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
