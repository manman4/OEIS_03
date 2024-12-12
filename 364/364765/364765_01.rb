def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = (1/n) * Sum_{k=0..floor(n-1)/2} binomial(n,k) * binomial(5*n-k,n-1-2*k) for n > 0.
def A(n)
  return 1 if n == 0
  (0..(n - 1) / 2).inject(0){|s, i| s + ncr(n, i) * ncr(5 * n - i, n - 1 - 2 * i)} / n
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
