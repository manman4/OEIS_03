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
  s = 0
  (1..n).each{|i|
    s += sigma(k, i) if n % i == 0
  }
  s
end

n = 140
cnt = 1
(1..n).each{|i|
  (1..i).each{|j|
    k = A(i - j, j)
    break if k.to_s.size > 1000
    print cnt
    print ' '
    puts k
    cnt += 1
  }
}


