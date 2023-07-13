def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, m, n)
  return 1 if n == 0
  (0..n - 1).inject(0){|s, i| s + (k + m) * ncr(n - 1, i) * ncr(k * n + m * i + 2, n - 1) / (k * n + m * i + 2r)}
end

n = 500
(0..n).each{|i|
  j = A(3, 2, i).to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
