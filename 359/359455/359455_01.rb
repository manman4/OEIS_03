def A(n)
  s = 0
  (1..n).each{|i|
    s += 1 if n % i == 0 && (n - i) % (i ** 3 ) == 0
  }
  s
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
} 