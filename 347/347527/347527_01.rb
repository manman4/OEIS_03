def A(k, n)
  s = 0
  (1..n).each{|i|
    s += 1 if i ** k <= n && n % i == 0
  }
  s
end

n = 10000
s = 0
(1..n).each{|i|
  s += A(4, i)
  print i
  print ' '
  puts s
}
