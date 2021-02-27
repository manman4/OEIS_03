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

def A(k, n)
  ary = (0..n).map{|i| f(i + k) / f(k)}
  sqrt_a(sqrt_a(ary))
end

n = 500
ary = A(7, n)
(0..n).each{|i|
  j = ary[i].to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
