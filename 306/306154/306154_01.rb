# mŸˆÈ‰º‚ğæ‚èo‚·
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

def I(ary, n)
  a = [1]
  (0..n - 1).each{|i| a << -(0..i).inject(0){|s, j| s += ary[1 + i - j] * a[j]}}
  a
end

# mŸˆÈ‰º‚ğæ‚èo‚·
def power(ary, n, m)
  return [1] if n == 0
  return I(power(ary, -n, m), m) if n < 0
  k = power(ary, n >> 1, m)
  k = mul(k, k, m)
  return k if n & 1 == 0
  return mul(k, ary, m)
end

def A(ary, n)
  a = [1] + [0] * n
  a_ary = []
  (1..n).each{|i|
    b_ary = Array.new(n + 1, 0)
    j = ary[i] - a[i]
    a_ary << j
    b_ary[0], b_ary[i] = 1, 1
    a = mul(a, power(b_ary, j, n), n)
  }
  a_ary
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 30
ary = (0..n).map{|i| (-i) ** i}
p A(ary, n)
