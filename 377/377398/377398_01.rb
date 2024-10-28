def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(0) = 1; a(n) = Sum_{k=1..n} (1 - 4 * k/n) * binomial(n,k) * a(n-k).
def A(n)
  ary = [1]
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + (1 - 4r * j / i) * ncr(i, j) * ary[i - j]}.to_i}
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
