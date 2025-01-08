def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(n) = n! * Sum_{k=0..n} (k+1) * (k+2)^(n-k)/(n-k)!.
def A(n)
  m = f(n)
  (0..n).inject(0){|s, k| s + (k + 1) * (k + 2) ** (n - k) * m / f(n - k)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
