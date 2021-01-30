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

def B(k, n)
  ary1 = ([0] + tau(k, n)).map{|i| -i}
  ary2 = Array.new(n + 1, -1)
  a = A(ary1, ary2, n)
end

n = 500
ary = [1] + (1..n).map{|i| B(i, i)[-1]}
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
