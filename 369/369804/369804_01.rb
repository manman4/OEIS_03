def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{k=0..floor(n/3)} binomial(n-1+2*k,n-3*k).
def A(n)
  (0..n / 3).inject(0){|s, i| s + ncr(n - 1 + 2 * i, n - 3 * i)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}