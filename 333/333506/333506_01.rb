def pow(a, m, mod)
  return 1 if m == 0
  k = pow(a, m >> 1, mod)
  k *= k
  return k % mod if m & 1 == 0
  return k * a % mod
end

def A127092(n)
  cnt = 0
  i = 1
  while cnt < n
    j = pow(19, i, i)
    if j == 1 % i
      cnt += 1
      print cnt
      print ' '
      puts i
    end
    i += 1
  end
end

n = 10000
A127092(n)
