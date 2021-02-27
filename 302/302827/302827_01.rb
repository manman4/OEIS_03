def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  (f(n) ** 2 * (1..n - 1).inject(0){|s, i| s + 1r / (i * (n - i)) ** 2}).to_i
end

n = 250
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
