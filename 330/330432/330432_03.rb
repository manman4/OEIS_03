# a(n) = Product_{k=1..n} (n - k + 2 - ceiling(n/k)).
def A(n)
  return 1 if n == 0
  (1..n).inject(1){|prod, k| prod * (n - k + 2 - (n.to_f / k).ceil)}
end

n = 1000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print " "
  print j
  puts
}