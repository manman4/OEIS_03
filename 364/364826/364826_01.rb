def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  s = 0
  (0..n).each{|i|
    s += (-3) ** i * ncr(n, i) * ncr(k * n + i + 1, n) / (k * n + i + 1r)
  }
  (-1) ** n * s.to_i
end

n = 1000
(0..n).each{|i|
  j = A(4, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}