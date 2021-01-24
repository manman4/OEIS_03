def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + k ** i * ncr(n, i) ** 2}
end

n = 400
(0..n).each{|i|
  j = A(i + 1, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
