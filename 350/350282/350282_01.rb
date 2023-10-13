def mul(f_ary, b_ary)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary
end

def power(ary, n)
  return [1] if n == 0
  k = power(ary, n >> 1)
  k = mul(k, k)
  return k if n & 1 == 0
  return mul(k, ary)
end

def A(n)
  a = [1]
  ary = [1]
  (1..n).each{|i|
    b = [1] + [0] * (2 * i - 1) + [1]
    a = mul(a, b)
    ary << power(a, i)[i * i * (i + 1) / 2]
  }
  ary
end

n = 20
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
