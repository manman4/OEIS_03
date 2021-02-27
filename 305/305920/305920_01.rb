require 'prime'

def power0(a, n)
  return 1 if n == 0
  k = power0(a, n >> 1)
  k *= k
  return k if n & 1 == 0
  return k * a
end

def sigma(x, i)
  sum = 1
  pq = i.prime_division
  pq.each{|a, n| sum *= (power0(a, (n + 1) * x) - 1) / (power0(a, x) - 1)}
  sum
end

def A(k, n)
  (1..n).map{|i| sigma(k, i)}
end

# m次以下を取り出す
def mul(f_ary, b_ary, m)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  s10 = [s1 - 1, m].min
  (0..s10).each{|i|
    s20 = [s2 - 1, m - i].min
    (0..s20).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary
end

# m次以下を取り出す
def power(ary, n, m)
  return [1] if n == 0
  k = power(ary, n >> 1, m)
  k = mul(k, k, m)
  return k if n & 1 == 0
  return mul(k, ary, m)
end

def A000594(n)
  ary = Array.new(n + 1, 0)
  # ヤコビの公式の必要なところだけ取り出す
  i = 0
  j, k = 2 * i + 1, i * (i + 1) / 2
  while k <= n
    i & 1 == 1? ary[k] = -j : ary[k] = j
    i += 1
    j, k = 2 * i + 1, i * (i + 1) / 2
  end
  # 8乗してx倍
  power(ary, 8, n).unshift(0)[1..n]
end

def A008408(n)
  ary1 = A(11, n)
  ary2 = A000594(n)
  [1] + (1..n).map{|i| 65520 * (ary1[i - 1] - ary2[i - 1]) / 691}
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

n = 400
a = A008408(n)
a0 = Array.new(n + 1, 1)
ary = inverse_Euler_transform(a, n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
