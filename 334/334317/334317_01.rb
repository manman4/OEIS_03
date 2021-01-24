def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A(n)
  ary = [1, 2]
  (2..n).each{|i| ary << 3 * (0..i - 1).inject(0){|s, j| s + ncr(i - 1, j) * ary[-j - 1] * ary[j]} / 2}
  ary
end

n = 500
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
