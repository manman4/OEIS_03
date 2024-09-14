def T(n, k)
  (k..n).select{|i| n % i == 0}.size
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts T(i, 6)
}
