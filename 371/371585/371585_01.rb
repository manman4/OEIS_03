def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n, r, s, t, u)
  return 1 if n == 0
  (r * (0..n).inject(0){|sum, i| sum + ncr(t * i + u * (n - i) + r, i) * ncr(n + (s - 1) * i - 1, n - i) / (t * i + u * (n - i) + r.to_r)}).to_i
end

n = 1000
(0..n).each{|i|
  j = A(i, 2, 2, 5, 0)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
