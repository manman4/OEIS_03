def A(m, n)
  k = m * m + n * n
  l = Math.sqrt(k).floor
  (l + 1) ** 2 - k
end

n = 10000
s = 0
t = 1
while s <= n
  print s
  print ' '
  puts t
  s += 1
  t = A(s, t)
end