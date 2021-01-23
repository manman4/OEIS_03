def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  ary = [1, 0, 0, 0, 0]
  (5..n).each{|i| ary << 2 * (5..i).inject(0){|s, j| s + ncr(i - 1, j - 1) * ary[-j]}}
  ary
end

n = 700
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
