def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

n = 1000
sum = 0
(0..n).each{|i|
  sum += ncr(4 * i + 1, i)
  break if sum.to_s.size > 1000
  print i
  print ' '
  puts sum
}
