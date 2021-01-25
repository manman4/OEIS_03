def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A002426(n)
  a = [1, 1]
  (2..n).each{|i| a << ((2 * i - 1) *a[-1] + 3 * (i - 1) * a[-2]) / i}
  a[0..n]
end

def A(k, n)
  a = A002426(n)
  (0..n).map{|i| (0..i).inject(0){|s, j| s + (-1) ** (i - j) * ncr(i, j) * a[j] ** k}}
end

n = 600
ary = A(4, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
