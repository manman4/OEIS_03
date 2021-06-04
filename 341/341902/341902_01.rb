def A(n)
  cnt = 2
  while (n ** 3 + cnt) % (n + cnt) > 0
    cnt += 1
  end
  cnt
end

n = 10000
(0..n).each{|i|
  print i
  print ' '
  puts A(i)
}