def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = 3/n*(3*n-2)*a(n-1)
def A004987(n)
  ary = [1]
  (1..n).each{|i| ary << 3 * (3 * i - 2) * ary[-1] / i}
  ary
end

# a(n) = Sum_{k=0..floor(n/3)} A004987(n-3*k) * binomial(3*n-8*k,k).
def A(n)
  a = A004987(n)
  ary = [1]
  (1..n).each{|i| ary << (0..i / 3).inject(0){|s, j| s + a[i - 3 * j] * ncr(3 * i - 8 * j, j)}}
  ary
end

n = 1100
m = 1000
ary = A(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
