def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A(k, n)
  ary = [1]
  (1..n).each{|i| ary << (0..i - 1).inject(0){|s, j| s + k ** j * ncr(i - 1, j) * ary[j]}}
  ary
end
def A309401(n)
  (0..n).map{|i| A(i, i)}
end

n = 500
(0..n).each{|i|
  j = A(i, i)[-1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
