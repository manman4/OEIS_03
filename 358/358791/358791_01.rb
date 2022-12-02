def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  (0..n / 2).inject(0){|s, i| s + f(2 * i) * f(n - 2 * i)}
end

n = 1000
(0..n).each{|i| 
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
