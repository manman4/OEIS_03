def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# Sum_{k=0..floor(n/6)} binomial(n-3*k,3*k) * binomial(3*k,k) / (2*k+1).
def A(n)
  (0..n / 6).inject(0){|s, i| s + ncr(n - 3 * i , 3 * i) * ncr(3 * i, i) / (2 * i + 1)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
