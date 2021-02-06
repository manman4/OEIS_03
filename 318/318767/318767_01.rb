def S(n)
  a = [0, 1]
  (2..n).each{|i|
    s = 1
    (1..i - 1).each{|j|
      s += a[j] if i % j == 0
    }
    a << s
  }
  [0, 1] + (2..n).map{|i| a[i] / 2}
end

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

def B(n)
  ary1 = S(n)
  ary2 = Array.new(n + 1, 1)
  p a = A(ary1, ary2, n)
  ary3 = ary1.map{|i| -i}
  ary4 = Array.new(n + 1, -1)
  p b = A(ary3, ary4, n)
  mul(a, b, n)
end

n = 100
p ary = B(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
