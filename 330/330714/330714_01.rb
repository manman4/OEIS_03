def A(n)
  s = 0
  n.to_s(2).split('').reverse.each_with_index{|e, i| s += e.to_i * 1i ** i}
  s.real ** 2 + s.imaginary ** 2
end

n = 10000
(0..n).each{|i| 
  print i
  print ' '
  puts A(i)
}