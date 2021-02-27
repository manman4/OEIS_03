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

def A(n)
  ary = [1]
  s_ary = [[1]]
  (1..n).each{|i|
    a = ary + [0] * i
    (0..(i - 1) * i / 2).each{|j| a[i + j] -= ary[j]}
    ary = a
    s_ary << power(ary, 2, 2 * i * (i + 1))
  }
  s_ary
end

n = 30
ary = A(n).flatten
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
