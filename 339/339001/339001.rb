def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A339001(n)
  (-1) ** (n % 2) * (0..n).inject(0){|s, i| s + (-n) ** i * ncr(n, i) * ncr(2 * i, i) / (i + 1)}
end

n = 500
(0..n).each{|i|
  j = A339001(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

