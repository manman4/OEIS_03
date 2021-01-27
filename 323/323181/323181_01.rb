def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A(n)
  [0] + (1..n).map{|i| (0..i - 1).inject(0){|s, j| s + ncr(2 * i, 2 * j + 1) * (i * i - 1) ** (i - 1 - j) * i ** (2 * j)} / 2}
end
n = 250
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
