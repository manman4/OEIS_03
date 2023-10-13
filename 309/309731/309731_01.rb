def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# Expansion of Sum_{j>=1} j * x^j/(1 - x^j)^k
def A(k, n)
  s = 0
  (1..n).each{|i|
    s += ncr(i + k - 2, k - 1) * (n / i) if n % i == 0
  }
  s
end

n = 10000
(1..n).each{|i|
  j = A(3, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}