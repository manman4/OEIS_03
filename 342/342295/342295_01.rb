def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end


def A(k, n)
  (0..n).inject(0){|s, i| s + ncr(n, i) ** k}
end

n = 1000
(0..n).each{|i|
  j = A(12, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
