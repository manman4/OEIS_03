def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

n = 1000
(0..n).each{|i|
  j = (0..i).inject(0){|s, j| s + ncr(2 * j - 1, j) * ncr(2 * j, i - j)}
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
