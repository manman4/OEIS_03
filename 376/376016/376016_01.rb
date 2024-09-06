def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = Sum_{d|n} d^(n/d) * binomial(n/d,d).
def A(n)
  s = 0
  (1..n).each{|i|
    if n % i == 0 && n / i >= i
      s += i ** (n / i) * ncr(n / i, i)
    end
  }
  s
end

n = 5000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}