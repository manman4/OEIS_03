def T(n)
  cnt = 0
  (0..Math.sqrt(n).to_i).each{|i|
    j = n - i * i
    cnt += 1 if Math.sqrt(j).to_i ** 2 == j
  }
  cnt
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
  p ary1 = (0..n).map{|i| -T(i)}
  ary4 = Array.new(n + 1, -1)
  A(ary1, ary4, n)
end

n = 10000
p ary = B(n)
(0..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
