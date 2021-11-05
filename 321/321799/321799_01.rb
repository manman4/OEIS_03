def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + ncr(n, i) * ncr(n + k * i - 1, n - i) / (n - i + 1r)}
end

n = 1000
(0..n).each{|i|
  j = A(4, i).to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

