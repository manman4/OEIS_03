# a(n) = Sum_{k=0..n} n^k * k^(n-k).
def A(n)
  (0..n).inject(0){|s, i| s + n ** i * i ** (n - i)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}