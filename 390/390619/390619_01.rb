def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..floor(n/3)} 2^k * 3^(n-3*k) * binomial(2*k,2*(n-3*k)).
def a(n)
  (0..n / 3).inject(0){|s, k| s + 2**k * 3**(n - 3 * k) * ncr(2 * k, 2 * (n - 3 * k))}
end

n = 1000
(0..n).each{|i| 
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
} 