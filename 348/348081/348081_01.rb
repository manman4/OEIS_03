def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = (1/(4*n)!) * Sum_{k=0..4*n} (-1)^k * binomial(4*n,k) * (2*n-k)^(6*n).
def A(n)
  (0..4 * n).inject(0){|s, i| s + (-1)**i * ncr(4 * n, i) * (2 * n - i)**(6 * n)} / f(4 * n)
end

n = 300
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
