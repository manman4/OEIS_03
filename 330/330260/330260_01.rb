def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A330260(n)
  (0..n).inject(0){|s, i| s + ncr(n, i) * n ** (n - i) * f(n) / f(i)}
end

n = 500
(0..n).each{|i|
  j = A330260(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}