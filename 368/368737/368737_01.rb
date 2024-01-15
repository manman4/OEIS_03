# a(n) = Sum_{k = 1..n} gcd(3*k, n).
def A(n)
  (1..n).inject(0){|s, i| s + n.gcd(3 * i)}
end

n = 10000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}