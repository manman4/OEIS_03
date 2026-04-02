# a(0) = 1, a(1) = a(2) = 0; a(n) = -a(n-1) + (n-2)*a(n-3) + Sum_{k=0..n-1} a(k) * a(n-1-k).
def A(n)
  a = [1, 0, 0]
  (3..n).each{|i| a << -a[-1] + (i - 2) * a[-3] + (0..i-1).inject(0){|s, k| s + a[k] * a[i-1-k]}}
  a
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