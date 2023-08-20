def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  m = f(n)
  (0..n).inject(0){|s, i| s + (1 / 2r) ** (n - i) * (k * i + 1) ** (i - 1) * ncr(i, n - i) * m / f(i)}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(1, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}