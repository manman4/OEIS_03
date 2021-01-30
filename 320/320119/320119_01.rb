# m���ȉ������o��
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

# m���ȉ������o��
def power(ary, n, m)
  return [1] if n == 0
  k = power(ary, n >> 1, m)
  k = mul(k, k, m)
  return k if n & 1 == 0
  return mul(k, ary, m)
end

def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end


n = 100
ary = Array.new(n + 1, 0)
(0..Math.sqrt(n).to_i).each{|i| ary[i * i] = 1}
a = ary.clone
(2..n).each{|i|
   b = Array.new(n + 1, 0)
   (0..n / i).each{|j| b[i * j] = ary[j]}
   a = mul(a, b, n)
}
p a
p a1 = I(a, n)
p power(a1, 3, n)
p power(a1, 12, n)
p power(a1, 24, n)

(0..1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
