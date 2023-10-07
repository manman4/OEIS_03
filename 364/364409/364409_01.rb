def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = (-1)^(n-1) * (1/n) * Sum_{k=0..n} binomial(n,k) * binomial(n+5*k-2,n-1) for n > 0.
def A(n)
  return 1 if n == 0
  (-1) ** ((n - 1) % 2) * (0..n).inject(0){|s, i| s + ncr(n, i) * ncr(n + 5 * i - 2, n - 1)} / n
end

def B(n)
  (0..n).map{|i| A(i)}
end

n = 1000
ary = B(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}