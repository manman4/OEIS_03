def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n, m)
  a = [1]
  (1..n).each{|i| a << (1..i).inject(0){|s, j| s + (0..j - 1).inject(1){|t, k| t * (-m * k + 1)} * ncr(i - 1, j - 1) * a[i - j]}}
  a
end

n = 500
ary = A(n, 3)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
