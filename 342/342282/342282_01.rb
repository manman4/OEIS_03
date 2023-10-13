def f(k, n)
  return 1 if k == 0
  (n + 1..n + k).inject(:*)
end

def B(n)
  a, b = 1, 2
  ary = [1]
  s = 1
  while s <= n
    s += 1
    a, b = b, ((7 * s * s + 21 * s + 12) * b + (8 * s * (s - 1)) * a)/((s + 3) * (s + 4))
    ary << a
  end
  ary
end

def I(ary, n)
  a = [1]
  (0..n - 1).each{|i| a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}}
  a
end

def A(n)
  ary = B(n)
  I(ary, n)
end

m = 1100
n = 1000
ary = A(m)
ary[0] = 0
(0..n).each{|i|
  j = -ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}