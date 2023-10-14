def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# ncr((k + 1) * i, i) / (k * i + 1)は整数
def A(k, n)
  return 1 if n == 0
  (1..n).inject(0){|s, i| s + (-1) ** ((n - i) % 2) * ncr(n - 1, i - 1) * ncr((k + 1) * i, i) / (k * i + 1)}
end

n = 500
(0..n).each{|i|
  j = A(6, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
