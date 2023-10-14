def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  return 1 if n == 0
  (1..n).inject(0){|s, i| s + ncr(n - 1, i - 1) * ncr(k * i, i) / ((k - 1) * i + 1)}
end

n = 1000
(0..n).each{|i|
  j = A(6, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}