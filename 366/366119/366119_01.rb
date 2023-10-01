def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, m, n)
  (0..n / k).inject(0){|s, i| s + (-1) ** (i % 2) * ncr(n + i, i) * ncr((m - 1) * n - i + m - 1, n - k * i)} / (n + 1)
end

n = 1000
(0..n).each{|i| 
  j = A(3, 5, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
