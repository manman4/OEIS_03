def f(n, k)
  return 1 if  n == k
  (k + 1..n).inject(:*)
end

def A(n)
  (0..n).inject(0){|s, i| s + f(n, i) * i ** (i * (n - i))}
end

n = 100
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
