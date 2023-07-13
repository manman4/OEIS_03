def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, m, n)
  (0..n).inject(0){|s, i| s + ncr(n, i) * ncr(k * n + m * i + 1, n) / (k * n + m * i + 1r)}
end

n = 500
(0..n).each{|i|
  j = A(3, 2, i).to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
