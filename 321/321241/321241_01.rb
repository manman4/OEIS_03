require 'prime'

def t(ary, n)
  a = [0] + ary
  b = []
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += a[j] if i % j == 0
    }
    b << s
  }
  b
end

def tau(k, n)
  ary = Array.new(n, 1)
  (k - 1).times{
    ary = t(ary, n)
  }
  ary
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

def B(k, n)
  ary0 = [0] + tau(k, n)
  ary1 = ary0.map{|i| -i}
  ary2 = Array.new(n + 1, -1)
  a = A(ary0, ary2, n)
  b = A(ary1, ary2.map{|i| -i}, n)
  mul(a, b, n)
end

n = 10000
ary = B(3, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
