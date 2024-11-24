def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# T(n,0) = 0^n and T(n,k) = k * Sum_{r=0..n} binomial(n,r) * binomial(2*r+k,n)/(2*r+k) for k > 0.
# A(n) = T(n,n) = n * Sum_{r=0..n} binomial(n,r) * binomial(2*r+n,n)/(2*r+n).
def A(n)
  return 1 if n == 0
  (n * (0..n).inject(0){|s, r| s + ncr(n, r) * ncr(2 * r + n, n) / (2 * r + n).to_r}).to_i
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}