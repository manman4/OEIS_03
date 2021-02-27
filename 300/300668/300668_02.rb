def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A(n)
  return 1 if n == 0
  s = 0
  (1..n).each{|i|
    s += phi(i) * 4 ** (n / i) if n % i == 0 && i % 2 == 1
  }
  s / (4 * n)
end

p (0..30).map{|i| A(i)}
