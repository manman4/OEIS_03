# m���ȉ������o��
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

# m���ȉ������o��
def power(ary, n, m)
  return [1] if n == 0
  k = power(ary, n >> 1, m)
  k = mul(k, k, m)
  return k if n & 1 == 0
  return mul(k, ary, m)
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

def A00(n)
  ary = [1] + [0] * n
  (1..n).each{|i|
    j = i * i
    break if j > n
    ary[j] = 2
  }
  power(ary, 4, n)
end

def A01(n)
  ary = [1] + [0] * n
  (1..n).each{|i|
    j = i * i
    break if j > n
    ary[j] = (-1) ** (i % 2) * 2
  }
  power(ary, 4, n)
end

def A10(n)
  ary = [1] + [0] * n
  (1..n).each{|i|
    j = i * (i + 1)
    break if j > n
    ary[j] = 1
  }
  power(ary, 4, n)
end

# -2�`n-2 
def A319294(n)
  a = A00(n)
  b = A01(n)
  c = A10(n)
  d = (0..n).map{|i| a[i] + b[i]}
  e = I(mul(c, c, n), n)
  f = mul(d, e, n).map{|i| i / 2}
  g = [1] + (1..n).map{|i| b[i] - 16 * c[i - 1]}
  h = mul(g, I(mul(a, a, n), n), n).map{|i| i * 128}
  ([1, 0] + (2..n).map{|i| f[i] + h[i - 2]})[0..n]
end

m = 10100
n = 10000
ary = A319294(m + 2)
(-2..n).each{|i|
  j = ary[i + 2]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
