def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n, k, q)
  s = 0
  (0..n / q - k).each{|j| s += (-1) ** j * f(n - (q - 1) * (j + k)) / f(j)}
  s / f(k)
end

n = 1000
(0..n).each{|i|
  j = A(i, 2, 2)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}