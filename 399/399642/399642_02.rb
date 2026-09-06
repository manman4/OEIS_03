def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = ((n+1)/8) * Sum_{k=0..n+1} 3^(n+1-k) * binomial(4*n+5,k).
def A(n)
  (0..n+1).inject(0){|sum, k| sum + 3**(n+1-k) * ncr(4*n+5, k) } * (n + 1) / 8
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
