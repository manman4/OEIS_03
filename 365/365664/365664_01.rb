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

# a(n) = (5*sigma_7(n) - (126*n-441)*sigma_5(n) + (756*n^2-4410*n+4935)*sigma_3(n) - (840*n^3-5880*n^2+9870*n-3229)*sigma(n))/967680. 
def A365664(n)
  s1, s3, s5, s7 = sigma(1, n), sigma(3, n), sigma(5, n), sigma(7, n)
  (5 * s7 - (126 * n - 441) * s5 + (756 * n * n - 4410 * n + 4935) * s3 - (840 * n * n * n - 5880 * n * n + 9870 * n - 3229) * s1) / 967680
end

n = 10000
(10..n).each{|i|
  j = A365664(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
