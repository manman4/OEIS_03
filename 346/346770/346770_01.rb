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

def A057660(n)
  sigma(2, n * n) / sigma(1, n * n)
end

def A346770(n)
  a = [0] + (1..n).map{|i| A057660(i)}
  ary = [1]
  (1..n).each{|i|
    ary << (1..i).inject(0){|s, j| s - a[j] * ary[-j]} / i
  }
  ary
end

n = 10000
ary = A346770(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
