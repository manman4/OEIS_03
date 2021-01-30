def S(k, n, a, b)
  return 1 if k == 1 && (n % b == 0) && (n / b >= a)
  s = 0
  (a..n / b).each{|i| s += S(k - 1, n, i, b * i)}
  s
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

def B4(n)
  ary1 = (0..n).map{|i| S(4, i, 1, 1)}
  ary4 = Array.new(n + 1, 1)
  A(ary1, ary4, n)
end

n = 10000
ary = B4(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
