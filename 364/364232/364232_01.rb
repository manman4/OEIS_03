def A(k, mod, n)
  s = 0
  (1..n).each{|i|
    s += (-1) ** (i + 1) * i if n % i == 0 && (n / i) % mod == k
  }
  s
end

n = 10000
(1..n).each{|i|
  j = A(1, 3, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}