def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} (k+1) * binomial(3*k+1,k) * binomial(3*n-2*k+1,n-k)/((3*k+1) * (3*n-2*k+1)).
def a(n)
  (0..n).inject(0){|s, k| s + (k + 1r) * ncr(3 * k + 1, k) * ncr(3 * n - 2 * k + 1, n - k) / ((3 * k + 1) * (3 * n - 2 * k + 1))}.to_i
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}