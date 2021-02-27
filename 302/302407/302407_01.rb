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

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

def A302407(n)
ary = A000521(n)
  ary[1] = 0
  c_ary = [0]
  (1..n).each{|i| c_ary << (1..i - 1).inject(i * ary[i]){|s, j| s - ary[j] * c_ary[-j]}}
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  a = [0]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += m_ary[i / j] * c_ary[j] if i % j == 0
    }
    a << s / i
  }
  a
end

n = 20
ary = A302407(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
