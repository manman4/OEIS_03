def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  (0..n).inject(0){|s, i| s + (-1) ** ((n - i) % 2) * ncr(n, i) * (0..n).inject(0){|t, j| t + ncr(i, j) ** 4}}
end

n = 900
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
