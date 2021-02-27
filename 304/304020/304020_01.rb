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

def A000521(n)
  s3 = [0] + (1..n + 1).map{|i| sigma(3, i)}
  s5 = [0] + (1..n + 1).map{|i| sigma(5, i)}
  ary = [1]
  (0..n).each{|i| ary << (1..i + 1).inject(0){|s, j| s + (504 * s5[j] - 240 * (i - j) * s3[j]) * ary[-j]} / (i + 1)}
  ary
end

# ary[0] = 1
def sqrt_a(ary)
  n = ary.size - 1
  a = [1]
  (0..n - 1).each{|i| a << (ary[i + 1] - (1..i).inject(0){|s, j| s + a[j] * a[-j]}) / 2}
  a
end

def A304020(n)
  a = A000521(n)
  a[1] = 0
  a = sqrt_a(a)
  sqrt_a(a)
end

n = 400
ary = A304020(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
