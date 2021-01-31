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

def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s += ary[1 + i - j] * a[j]}
    i += 1
  end
  a
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

# -2`n-2
def A(n)
  b = A01(n)
  c = A10(n)

  d0 = power(b, 5, n)
  e0 = mul(c, c, n)
  f0 = mul(d0, e0, n)

  d1 = power(b, 6, n)
  f1 = mul(d1, c, n)

  f2 = power(b, 7, n)

  g = [2, 0] + (2..n).map{|i| 7 * 256 * f0[i - 2] + 7 * 16 * f1[i - 1] + 2 * f2[i]}
  r_ary0 = A000594(n + 1)
  r_ary = Array.new(n + 1, 0)
  (0..n / 2).each{|i| r_ary[2 * i] = r_ary0[i]}

  mul(g, I(mul(r_ary, r_ary, n), n), n)
end

n = 10100
ary = A(n + 4).map{|i| i / 2}
(-4..10000).each{|i|
  j = ary[i + 4]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
