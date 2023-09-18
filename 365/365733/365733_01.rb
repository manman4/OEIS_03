def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# Sum_{k=0..floor(n/6)} binomial(n-2*k,4*k) * binomial(4*k,k) / (3*k+1).
def A(n)
  (0..n / 6).inject(0){|s, i| s + ncr(n - 2 * i , 4 * i) * ncr(4 * i, i) / (3 * i + 1)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
