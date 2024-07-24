require 'prime'

def power0(a, n)
  return 1 if n == 0
  k = power0(a, n >> 1)
  k *= k
  return k if n & 1 == 0
  return k * a
end

# x > 0
def sigma(x, i)
  sum = 1
  pq = i.prime_division
  pq.each{|a, n| sum *= (power0(a, (n + 1) * x) - 1) / (power0(a, x) - 1)}
  sum
end

# a(n) = (7*sigma_3(n) - (6*n+1)*sigma(n))/24.
def A(n)
  s1, s3 = sigma(1, n), sigma(3, n)
  (7 * s3 - (6 * n + 1) * s1) / 24
end

n = 200
b=[]
(2..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
  b<<j
}
p b
