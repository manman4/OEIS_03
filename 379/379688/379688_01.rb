def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(n) = (1/(n+1)) * Sum_{k=0..n} 2^(n-k) * (n-k)^k * (2*n-k)!/(k! * (n-k)!).
def A(n)
  ((0..n).inject(0){|s, i| s + 2 ** (n - i) * (n - i) ** i * f(2 * n - i) / (f(i) * f(n - i).to_r)} / (n + 1)).to_i
end

n = 500
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
