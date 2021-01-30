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

def tau(n)
  ary = Array.new(n, 1)
  a = [1]
  (n - 1).times{|i|
    ary = t(ary, n)
    a << ary[i + 1]
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

def B(n)
  ary1 = ([0] + tau(n)).map{|i| -i}
  ary2 = Array.new(n + 1, -1)
  A(ary1, ary2, n)
end

n = 1000
ary = B(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
