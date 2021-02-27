def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def sqrt_a(ary)
  n = ary.size - 1
  a = [1]
  (0..n - 1).each{|i| a << (ary[i + 1] - (1..i).inject(0){|s, j| s + a[j] * a[-j]}) / 2}
  a
end

def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

def A(k, n)
  l = 2 ** k
  m = 2 * l - 1
  ary = (0..n).map{|i| f(i + m) / f(m)}
  k.times{ ary = sqrt_a(ary)}
  I(ary, n)
end

n = 500
ary = A(3, n)
(0..n).each{|i|
  j = ary[i].to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
