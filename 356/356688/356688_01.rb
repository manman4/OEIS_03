def f(n, k)
  return 1 if  n == k
  (k + 1..n).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + f(n, i) * i ** (k * n)}
end

n = 200
(0..n).each{|i|
  j = A(3, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
