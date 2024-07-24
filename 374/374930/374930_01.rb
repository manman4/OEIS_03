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

# a(n) = (31*sigma_5(n) - 70*(n+1)*sigma_3(n) + (40*n^2+60*n+9)*sigma(n))/1920.
def A(n)
  s1, s3, s5 = sigma(1, n), sigma(3, n), sigma(5, n)
  (31 * s5 - 70 * (n + 1) * s3 + (40 * n * n + 60 * n + 9) * s1) / 1920
end

n = 200
b=[]
(3..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
  b<<j
}
p b
