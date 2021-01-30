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

# m次以下を取り出す
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

def A(n)
  a = E_2k(2, n)
  b = Array.new(n + 1, 0)
  (0..n / 2).each{|i| b[2 * i] = a[i]}
  b2 = mul(b, b, n)
  c = (0..n).map{|i| a[i] - b[i]}
  c2 = mul(c, c, n)
  (0..n).map{|i| b2[i] + c2[i] / 15}
end

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

# ary[0] = 1
def inverse_Euler_transform(ary, n)
  c = [0]
  (1..n).each{|i| c << (1..i - 1).inject(i * ary[i]){|s, j| s -= ary[j] * c[-j]}}
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  a = [0]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += m_ary[i / j] * c[j] if i % j == 0
    }
    # aryの要素が整数なら、s / iは整数
    a << s / i
  }
  a
end

n = 600
a = A(n)
ary = inverse_Euler_transform(a, n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}