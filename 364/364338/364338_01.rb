def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, m, n)
  (0..n).inject(0){|s, i| s + ncr(k * n + m * i + 1, i) * ncr(k * n + m * i + 1, n - i) / (k * n + m * i + 1).to_r}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(0, 5, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
