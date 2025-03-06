def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n, k)
  ary = [1]
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + ncr(2 * k * j - 1, 2 * j) * ary[i - j]} / i}
  ary
end

n = 550
ary = A(n, 5)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}