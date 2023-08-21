def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} binomial(t*n-(t-1)*(k-1),k) * binomial(n+(s-1)*k-1,n-k)/(n-k+1)
def A(s, t, n)
  return 1 if n == 0
  (0..n).inject(0){|sum, i| sum + ncr(t * n - (t - 1) * (i - 1), i) * ncr(n + (s - 1) * i - 1, n - i) / (n - i + 1r)}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(3, 4, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}