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

def A340715(n)
  f = File.open("b058072.txt")
  str = f.read  # 全て読み込む
  f.close
  str.split(/\R/).each{|i|
    m = i.split(' ')[-1].to_i
    return m if sigma(1, m + 1) == n * sigma(1, m)
  }
end

n = 5
(1..n).each{|i|
  print i
  print ' '
  puts A340715(i)
}