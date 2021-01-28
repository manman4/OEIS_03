def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A(n)
  (0..n).map{|i| (0..i).inject(-1){|s, j| s + ncr(2 * i, 2 * j) * (i + 1) ** (i - j) * i ** j} / 2}
end
n = 400
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
