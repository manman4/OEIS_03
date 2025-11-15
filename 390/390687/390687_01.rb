def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..floor(n/2)} (-1)^k * binomial(4*n-2*k,n-2*k).
def a(n)
  (0..n / 2).inject(0){|s, k| s + (-1)**k * ncr(4 * n - 2 * k, n - 2 * k)}
end

n = 1000
(0..n).each{|i|
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
