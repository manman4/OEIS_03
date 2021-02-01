def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A340971(n)
  (0..n).inject(0){|s, i| s + n ** i * ncr(n, i) * ncr(2 * i, i)}
end

n = 500
(0..n).each{|i|
  j = A340971(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

