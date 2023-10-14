def A(n)
  return 0 if n == 2
  m = n ** n
  cnt = 2
  while (m + cnt) % (n + cnt) > 0
    cnt += 1
  end
  cnt
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}