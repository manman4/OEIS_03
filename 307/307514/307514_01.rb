def s(f_ary, g_ary, n)
  s = 0
  (1..n).each{|i| s += i * f_ary[i] * g_ary[i] ** (n / i) if n % i == 0}
  s
end
def A(f_ary, g_ary, n)
  ary = [1]
  a = [0] + (1..n).map{|i| s(f_ary, g_ary, i)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s += a[j] * ary[-j]} / i}
  ary
end
def A307514(n)
  A((0..n).map{|i| (-1) ** (i % 2 + 1) * i ** i}, Array.new(n + 1, 1), n)
end
n = 500
ary = A307514(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
