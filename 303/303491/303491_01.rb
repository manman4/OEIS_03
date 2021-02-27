def s(f_ary, g_ary, n)
  s = 0
  (1..n).each{|i| s += i * f_ary[i] * g_ary[i] ** (n / i) if n % i == 0}
  s
end

def A(f_ary, g_ary, n)
  ary = [1]
  a = [0] + (1..n).map{|i| s(f_ary, g_ary, i)}
  b = [0] + (1..n).map{|i| s(f_ary.map{|i| -i}, g_ary.map{|i| -i}, i)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s - (a[j] + b[j]) * ary[-j]} / i}
  ary
end

def B(n)
  ary1 = (0..n).map{|i| 1r / 8 ** i}
  ary2 = (0..n).map{|i| 8 ** i}
  ary = A(ary1, ary2, n)
end

n = 100
ary = B(n)
(0..n).each{|i|
  j = ary[i].to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
