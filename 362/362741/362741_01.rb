def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=ceiling(n/2)..n} A000108(k)*binomial(n,k)*binomial(k,n-k)/(n-k+1).
def A(n)
  (n / 2..n).inject(0){|s, i| s + ncr(2 * i, i) / (i + 1) * ncr(n, i) * ncr(i, n - i) / (n - i + 1r)}.to_i
end

n = 1000
(0..n).each{|i| 
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
