def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(n) = Sum_{k=0..n} (n - k)! * k! / floor(k / 2)!^2
def A(n)
  (0..n).inject(0){|s, i| s + f(n - i) * f(i) / f(i / 2).to_r ** 2}.to_i
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}