def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A(k, n)
  (0..n).map{|i| (0..k).inject(-1){|s, j| s + ncr(2 * k, 2 * j) * (i + 1) ** (k - j) * i ** j} / 2}
end

n = 37
p ary = A(3, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
