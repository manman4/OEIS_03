def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..floor(n/4)} 2^k * 3^(n-3*k) * binomial(2*(n-3*k),2*k).
def a(n)
  (0..n / 4).inject(0){|s, k| s + 2**k * 3**(n - 3 * k) * ncr(2 * (n - 3 * k), 2 * k)}
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 