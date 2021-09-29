def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  return 1 if n == 0
  -(0..2 * n - 1).inject(0){|s, i| s + (-1) ** (i % 2) * (2 * i + 1) ** (3 * n - 1) * ncr(2 * n - 1, i)} / (2 ** (2 * n - 1) * f(2 * n - 1))
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
