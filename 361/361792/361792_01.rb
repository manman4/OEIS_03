def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n, k)
  (0..n).inject(0){|s, i| s + (-1) ** (n - i) * ncr(2 * i, i) * ncr(n + (k - 1) * i - 1, n - i)}
end

n = 1000
(0..n).each{|i|
  j = A(i, 6)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}