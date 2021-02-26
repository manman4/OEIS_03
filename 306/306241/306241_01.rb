def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  (0..n).inject(0){|s, i| s + f(i * n) / f(n) ** i}
end

n = 50
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
