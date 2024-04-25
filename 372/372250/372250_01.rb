def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(n) = n! * Sum_{k=0..floor(n/3)} (n-k+1)^(n-2*k-1) / (k! * (n-3*k)!).
def A(n)
  m = f(n)
  (0..n / 3).inject(0){|s, k| s + (n - k + 1) ** (n - 2 * k - 1) * m / (f(k) * f(n - 3 * k))}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}