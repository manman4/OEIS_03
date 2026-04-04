# a(0) = 1; a(n) = -3*a(n-1) + (n+7)/2 * Sum_{k=0..n-1} a(k) * a(n-1-k).
def A(n)
  ary = [1]
  (1..n).each{|i| ary << -3 * ary[-1] + (0..i - 1).inject(0){|s, j| s + ary[j] * ary[i - 1 - j]} * (i + 7) / 2}
  ary
end

n = 600
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}