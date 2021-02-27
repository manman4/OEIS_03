def s(f_ary, g_ary, n, m)
  s = 0
  (1..[n, m].min).each{|i| s += i * f_ary[i] * g_ary[i] ** (n / i) if n % i == 0}
  s
end

def A(f_ary, g_ary, n, m)
  ary = [1]
  a = [0] + (1..n).map{|i| s(f_ary, g_ary, i, m)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + a[j] * ary[-j]} / i}
  ary
end

(1..100).each{|n|
  n0 = n ** 3 - n * n
  ary1 = Array.new(n0 + 1, 1)
  ary2 = Array.new(n0 + 1, 1)
  p A(ary1, ary2, n0, n * n)[-1]
}
