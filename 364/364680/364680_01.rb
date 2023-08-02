require 'prime'

def power0(a, n)
  return 1 if n == 0
  k = power0(a, n >> 1)
  k *= k
  return k if n & 1 == 0
  return k * a
end

def sigma(i)
  sum = 1
  pq = i.prime_division
  pq.each{|a, n| sum *= (power0(a, n + 1) - 1) / (a - 1)}
  sum
end

# sigma(k) > sigma(k+1) & sigma(k+1) > sigma(k+2) &  ... sigma(k+n-2) > sigma(k+n-1)となる最小のkを返す
def A(n)
  k = 1
  while !(0..n - 2).all?{|i| sigma(k + i) > sigma(k + i + 1)}
    k += 1
  end
  k
end

n = 4
(1..n).each{|i| p [i, A(i)]}

