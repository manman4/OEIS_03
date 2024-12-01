def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n, s=4, t=3, u=1) = sum(k=0, n, binomial(t*k+u*(n-k)+1, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+1));
def A(n, s, t, u)
  return 1 if n == 0
  (0..n).inject(0){|sum, i| sum + ncr(t * i + u * (n - i) + 1, i) * ncr(n + (s - 1) * i - 1, n - i) / (t * i + u * (n - i) + 1r)}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(i, 4, 3, 1)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
