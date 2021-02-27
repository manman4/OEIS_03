def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  (f(n) ** 3 * (1..n - 1).inject(0){|s, i| s + 1r / (i * (n - i)) ** 3}).to_i
end

n = 10
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
