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

# a[0]=0
def sigma_ary(n)
  [0] + (1..n).map{|i| sigma(1, i)}
end

def A(k, n)
  s_ary = sigma_ary(n)
  ary = []
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += k ** (s_ary[j] - 1) if i % j == 0
    }
    ary << s
  }
  ary
end

n = 1000
ary = A(2, n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
