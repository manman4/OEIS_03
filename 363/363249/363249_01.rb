def A(k, n)
  (k ** n).to_s[0].to_i
end

n = 10000
(0..n).each{|i|
  print i
  print ' '
  puts A(9, i)
}