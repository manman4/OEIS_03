def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  return 1 if n == 0
  (1..n).inject(0){|s, i| s + ncr(n, i) * ncr(n + (k - 1) * i, i - 1)} / n
end

n = 500
(0..n).each{|i|
  j = A(i, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
