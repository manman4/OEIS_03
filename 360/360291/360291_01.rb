def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n / k).inject(0){|s, i| s + ncr(n - 1 - (k - 1) * i, i) * ncr(2 * (n - k * i), n -  k * i)}
end

n = 1000
(0..n).each{|i|
  j = A(3, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}