def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + ncr(n + (k - 1) * i, k * i) * ncr(k * i, i) / ((k - 1) * i + 1r)}
end

n = 500
(0..n).each{|i|
  j = A(7, i).to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
