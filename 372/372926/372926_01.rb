require 'prime'

def A008663(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

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

# k > 0, a(n) = Sum_{d|n} mu(n/d) * d^m * sigma_k(d)
def A(k, m, n)
  m_ary = [0] + (1..n).map{|i| A008663(i)}
  s_ary = [0] + (1..n).map{|i| sigma(k, i)}
  ary = [0]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += m_ary[i / j] * j ** m * s_ary[j] if i % j == 0
    }
    ary << s
  }
  ary
end

n = 10000
ary = A(2, 2, n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}