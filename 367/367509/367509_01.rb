def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def sigma(k, n)
  s = 0
  (1..n).each{|i| s += i ** k if n % i == 0}
  s
end

# a(n) = Sum_{k=0..n} 2^k * binomial(n,k) * sigma_k(n).
def A(k, n)
  (0..n).inject(0){|s, i| s + 2 ** i * ncr(n, i) * sigma(i, n)}
end

n = 1000
(1..n).each{|i|
  j = A(2, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}