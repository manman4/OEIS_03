def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# (1/(2^(4*n-1)*(4*n-1)!) * Sum_{k=0..4*n-1} (-1)^k * (4*n-1-2*k)^(6*n-1) * binomial(4*n-1,k) for n > 0.
def A(n)
  return 1 if n == 0
  (0..4 * n - 1).inject(0){|s, i| s + (-1)**i * (4 * n - 1 - 2 * i)**(6 * n - 1) * ncr(4 * n - 1, i)} / (2**(4 * n - 1) * f(4 * n - 1))
end

n = 200
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

