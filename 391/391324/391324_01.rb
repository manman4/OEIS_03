def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..floor(n/3)} (-1)^k * (2*k+1) * binomial(4*n-10*k+1,n-3*k)/(4*n-10*k+1).
def a(n)
  (0..n / 3).inject(0){|s, k| s + (-1)**k * (2 * k + 1) * ncr(4 * n - 10 * k + 1, n - 3 * k) / (4 * n - 10 * k + 1r)}.to_i
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 