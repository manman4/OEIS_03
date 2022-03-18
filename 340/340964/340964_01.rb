require 'prime'

# m次以下を取り出す
def mul(f_ary, b_ary, m)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary[0..m]
end

# m次以下を取り出す
def power(ary, n, m)
  return [1] if n == 0
  k = power(ary, n >> 1, m)
  k = mul(k, k, m)
  return k if n & 1 == 0
  return mul(k, ary, m)
end

def A(k, n)
  ary = [0] * (n + 1)
  (1..n).each{|i|
    ary[i] = 1 if i.prime?
  }
  power(ary, k, n)
end

n = 10100
m = 10000
ary = A(8, n)
(16..m).each{|i|
  print i
  print ' '
  puts ary[i]
}