require 'prime'

def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def pow(a, m, mod)
  return 1 % mod if m == 0
  k = pow(a, m >> 1, mod)
  k *= k
  return k % mod if m & 1 == 0
  return k * a % mod
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts pow(2, phi(i), i)
}
