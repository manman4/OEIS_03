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

# -1����n
def A000521(n)
  s3 = [0] + (1..n + 1).map{|i| sigma(3, i)}
  s5 = [0] + (1..n + 1).map{|i| sigma(5, i)}
  ary = [1]
  (0..n).each{|i| ary << (1..i + 1).inject(0){|s, j| s + (504 * s5[j] - 240 * (i - j) * s3[j]) * ary[-j]} / (i + 1)}
  ary
end

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

# ary[0] = 1
def inverse_Euler_transform(ary, n)
  c = [0]
  (1..n).each{|i| c << (1..i - 1).inject(i * ary[i]){|s, j| s - ary[j] * c[-j]}}
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  a = [0]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += m_ary[i / j] * c[j] if i % j == 0
    }
    # ary�̗v�f�������Ȃ�As / i�͐���
    a << s / i
  }
  a
end

def s(f_ary, g_ary, n)
  s = 0
  (1..n).each{|i| s += i * f_ary[i] * g_ary[i] ** (n / i) if n % i == 0}
  s
end

def A(f_ary, g_ary, n)
  ary = [1]
  a = [0] + (1..n).map{|i| s(f_ary, g_ary, i)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + a[j] * ary[-j]} / i}
  ary
end

n = 30
ary0 = Array.new(n + 1, 1)
ary1 = A000521(n - 1)
ary1[1] = 24
ary2 = inverse_Euler_transform(ary1, n).map{|i| i / 24}
ary =  A(ary2, ary0, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
