def A(n)
  s = n
  t = n + 1
  while s % t > 0
    s += t
    t += 1
  end
  s
end

n = 10000
(3..n).each{|i| 
  print i
  print ' '
  puts A(i)
}