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

def A(n)
  ary = []
  i = 1
  while ary.size < n
    if sigma(1, i) < sigma(1, i + 1) && sigma(1, i + 1) < sigma(1, i + 2)
      ary << i
    end
    i += 1
  end
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}