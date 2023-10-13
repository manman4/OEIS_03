def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  m = f(n)
  (1..n).inject(0){|s, i| s + m / (n / i)}
end

n = 1000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}