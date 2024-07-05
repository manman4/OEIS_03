require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
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

def f(n, k)
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  ary = [1]
  # b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);
  (1..n).each{|i|
    s = 0
    (1..i).each{|d|
      s += m_ary[d] * k ** (i / d) if i % d == 0 && d.gcd(k) == 1
    }
    ary << s / (k * i)
  }
  ary
end

n = 1100
m = 1000

ary1 = f(n, 5).map{|i| 5 * i}
ary2 = Array.new(n + 1, 1)

ary = A(ary1, ary2, n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
