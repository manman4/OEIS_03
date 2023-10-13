def pow(a, m, mod)
  return 1 % mod if m == 0
  k = pow(a, m >> 1, mod)
  k *= k
  return k % mod if m & 1 == 0
  return k * a % mod
end

def A(n)
  cnt = 0
  (1..n - 1).each{|i|
    cnt += 1 if (pow(i, n - i, n) + pow(n - i, i, n)) % n == 0
  }
  cnt
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}
