require 'prime'

def pow(a, m, mod)
  return 1 % mod if m == 0
  k = pow(a, m >> 1, mod)
  k *= k
  return k % mod if m & 1 == 0
  return k * a % mod
end

def d(m, k)
  return m unless @factor[k]
  (0..@factor[k][1]).map{|e| d(m * @factor[k][0] ** e, k + 1)}.flatten
end

def A(n)
  return 0 if n == 1
  s = 0
  @factor = Prime.prime_division(n)
  d(1, 0).each{|i|
    s += pow(i, i, n) if n % i == 0
  }
  s % n
end

n = 103703
cnt = 0
(1..n).each{|i|
  if A(i) == (1 % i)
    cnt += 1
    print cnt
    print ' '
    puts i
  end
}