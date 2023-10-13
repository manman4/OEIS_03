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
  s = 0
  @factor = Prime.prime_division(n)
  d(1, 0).each{|i|
    s += pow(i, i, n) if n % i == 0
  }
  s % n
end

(2..100000000).each{|i|
  if A(i) == (1 % i)
    p_ary = i.prime_division
    if p_ary.size > 1
      p i
    else
      pr, e = p_ary[0]
      if e > pr
        p i
      end
    end
  end
}