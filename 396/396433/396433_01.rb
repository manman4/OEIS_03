def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..n} (2*k+1) * binomial(4*k+1,k) * binomial(4*n-2*k+1,n-k)/((4*k+1) * (4*n-2*k+1)).
def a(n)
  (0..n).inject(0){|s, k| s + (2*k + 1r) * ncr(4*k + 1, k) * ncr(4*n - 2*k + 1, n - k) / ((4*k + 1) * (4*n - 2*k + 1))}.to_i
end

n = 1000
(0..n).each{|i|
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
