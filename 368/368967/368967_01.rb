def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n, s, t, u)
  (0..n / s).inject(0){|sum, i| sum + ncr(t * (n + 1) + i - 1, i) * ncr((t + u + 1) * (n + 1) - (s - 1) * i - 2, n - s * i)} / (n + 1)
end

n = 1000
(0..n).each{|i|
  j = A(i, 2, 2, 2)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}