# mŸˆÈ‰º‚ğæ‚èo‚·
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

# mŸˆÈ‰º‚ğæ‚èo‚·
def power(ary, n, m)
  return [1] if n == 0
  k = power(ary, n >> 1, m)
  k = mul(k, k, m)
  return k if n & 1 == 0
  return mul(k, ary, m)
end

def A(k, n)
  ary = [1] + Array.new(n, 0)
  (1..Math.sqrt(n).to_i).each{|i| ary[i * i] = 2}
  power(ary, k, n)
end

n = 10000
ary = A(16, n)
(1..n).each{|i|
  j = ary[i] / 32
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
