def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = ((n+1)/14) * Sum_{k=0..n+1} 6^(n+1-k) * binomial(7*n+8,k).
def A(n)
  (0..n+1).inject(0){|sum, k| sum + 6**(n+1-k) * ncr(7*n+8, k) } * (n + 1) / 14
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
