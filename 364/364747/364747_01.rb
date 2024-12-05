def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = r * Sum_{k=0..n} binomial(t*k+u*(n-k)+r,k) * binomial(n+(s-1)*k-1,n-k)/(t*k+u*(n-k)+r)
def A(n, r, s, t, u)
  (0..n).inject(0){|sum, k| sum + ncr(t * k + u * (n - k) + r, k) * ncr(n + (s - 1) * k - 1, n - k) / (t * k + u * (n - k) + r.to_r)}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(i, 1, 1, 4, 1)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
