def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} (-1)^k * binomial(n+k+1,k) * binomial(n+k+1,n-k) / (n+k+1).
def A(n)
  (0..n).inject(0){|s, i| s + (-1) ** i * ncr(n + i + 1, i) * ncr(n + i + 1, n - i) / (n + i + 1r)}.to_i
end

n = 1000
(0..n).each{|i| 
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
