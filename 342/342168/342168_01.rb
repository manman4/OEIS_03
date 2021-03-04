def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + (n + k) ** i * ncr(n + 1 + i, 2 * i + 1)}
end

n = 1000
(0..n).each{|i|
  j = A(1, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
