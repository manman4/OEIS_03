require 'prime'

def power0(a, n)
  return 1 if n == 0
  k = power0(a, n >> 1)
  k *= k
  return k if n & 1 == 0
  return k * a
end

# x > 0
def sigma(x, i)
  sum = 1
  pq = i.prime_division
  pq.each{|a, n| sum *= (power0(a, (n + 1) * x) - 1) / (power0(a, x) - 1)}
  sum
end

def bernoulli(n)
  ary = []
  a = []
  (0..n).each{|i|
    a << 1r / (i + 1)
    i.downto(1){|j| a[j - 1] = j * (a[j - 1] - a[j])}
    ary << a[0] # Bn = a[0]
  }
  ary
end

def E_2k(k, n)
  a = -4 * k / bernoulli(2 * k)[-1]
  b = a.denominator
  c = a.numerator
  [b] + (1..n).map{|i| c * sigma(2 * k - 1, i)}
end

# mŽŸˆÈ‰º‚ðŽæ‚èo‚·
def mul(f_ary, b_ary, m)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary[0..m]
end

# mŽŸˆÈ‰º‚ðŽæ‚èo‚·
def power(ary, n, m)
  return [1] if n == 0
  k = power(ary, n >> 1, m)
  k = mul(k, k, m)
  return k if n & 1 == 0
  return mul(k, ary, m)
end

def s(n)
  s = 0
  (1..n).each{|i| s += i if n % i == 0}
  s
end

def A000594(n)
  ary = [1]
  a = [0] + (1..n - 1).map{|i| s(i)}
  (1..n - 1).each{|i| ary << (1..i).inject(0){|s, j| s -= 24 * a[j] * ary[-j]} / i}
  ary
end

def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s += ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

# 3`nŽŸ
def N(n)
  a = E_2k(1, n + 1)
  b = E_2k(2, n + 1)
  c = E_2k(3, n + 1)
  d0 = mul(b, b, n)
  p d1 = mul(d0, d0, n)
  p d2 = mul(mul(c, c, n), b, n)
  p d3 = mul(c, mul(a, d0, n), n)
 p e = mul(a, a, n)
p power(b, 3, n)
p ""
  p d4 = mul(power(b, 3, n), e, n)
p power(c, 2, n)
p ""
  p d5 = mul(power(c, 2, n), e, n)
  (3..n).map{|i| 25 * d1[i] - 49 * d2[i] + 48 * d3[i] - 49 * d4[i] + 25 * d5[i]}
end

def A319134(n)
  a = N(n + 2)
  b = A000594(n)
  c = I(mul(b, b, n - 1), n - 1)
  mul(a, c, n - 1).map{|i| -i / 3657830400}
end

def A319134_2(n)
  a = N(n + 2)
  b = A000594(n)
  c = I(mul(b, b, n - 1), n - 1)
  mul(a, c, n - 1).map{|i| -i % 3657830400}
end

n = 20
p A319134_2(n)
=begin
ary = A319134(n)
(1..50).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
=end
