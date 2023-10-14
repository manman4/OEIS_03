def f(n)
  return 1 if n == 0
  (1..n).inject(:*)
end

def A(n)
  m = f(n)
  (0..n).inject(0){|s, i| s + (-1) ** (n - i) * i ** n * m / f(n - i)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}