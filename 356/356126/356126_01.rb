require 'prime'

def power0(a, n)
  return 1 if n == 0
  k = power0(a, n >> 1)
  k *= k
  return k if n & 1 == 0
  return k * a
end

def sigma(x, i)
  return (1..i).select{|j| i % j == 0}.size if x == 0
  sum = 1
  pq = i.prime_division
  pq.each{|a, n| sum *= (power0(a, (n + 1) * x) - 1) / (power0(a, x) - 1)}
  sum
end

def A(k, n)
  ary = [0] + (1..n).map{|i| sigma(k - 1, i) * i}
  a = [0]
  s = 0
  (1..n).each{|i|
    s += ary[i]
    a << s
  }
  a
end

n = 10100
m = 10000
ary = A(4, n)
(1..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
