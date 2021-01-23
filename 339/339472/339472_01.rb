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
  flag = false
  m = sigma(1, n)
  (1..n).each{|i|
    if n % i == 0
      flag = true if m == (i * sigma(1, i))
    end
  }
  flag
end

def A339472(n)
  ary = []
  i = 1
  while ary.size < n
    ary << i if A(i)
    i += 1
  end
  ary
end

n = 100
ary = A339472(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
