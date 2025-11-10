def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

n = 1000
sum = 0
(0..n).each{|i|
  sum += (-1) ** (i % 2) * ncr(3 * i + 1, i)
  j = (-1) ** (i % 2) * sum
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
