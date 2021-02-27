def s(f_ary, g_ary, n)
  s = 0
  (1..n).each{|i| s += i * f_ary[i] * g_ary[i] ** (n / i) if n % i == 0}
  s
end

def A(f_ary, g_ary, n)
  ary = [1]
  a = [0] + (1..n).map{|i| s(f_ary, g_ary, i)}
  b = [0] + (1..n).map{|i| s(f_ary.map{|i| -i}, g_ary.map{|i| -i}, i)}
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + (a[j] + b[j]) * ary[-j]} / i}
  ary
end

def B(k, n)
  ary1 = (0..n).map{|i| 1r / k}
  ary2 = (0..n).map{|i| k ** i}
  ary = A(ary1, ary2, n)
end

n = 64
(1..n).each{|i|
  ary = B(i, n)
  p [i, ary.map{|i| i.to_i}] if (0..n).all?{|i| ary[i].denominator == 1}
}
