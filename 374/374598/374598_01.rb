def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n / (k + 1)).inject(0){|s, i| s + 2 ** i * ncr(n - k * i, i) * ncr(2 * (n - k * i), n -  k * i)}
end

n = 1000
(0..n).each{|i|
  j = A(2, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}