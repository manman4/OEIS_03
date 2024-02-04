def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..floor(n/2)} binomial(n-1+3*k,n-2*k).
def A(n)
  (0..n / 2).inject(0){|s, i| s + ncr(n - 1 + 3 * i, n - 2 * i)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}