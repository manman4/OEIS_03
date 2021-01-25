def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + ncr(n, i) * (0..i).inject(0){|t, j| t + ncr(i, j) ** k}}
end

n = 100
(0..n).each{|i|
  j = A(i, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
