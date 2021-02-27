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

def A(k, n)
  ary = [1]
  s = 1
  (0..n).each{|i|
    s += k ** i
    b_ary = Array.new(k ** i + 1, 0)
    b_ary[0], b_ary[-1] = 1, -1
    ary = mul(ary, b_ary, s)
  }
  ary
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

def B(k, m, n)
  a = A(k, n)
  s = a.size - 1
  b = Array.new(s + 1, 0)
  (0..s / m).each{|i| b[m * i] = a[i]}
  mul(b, I(a, s), s)
end

n = 4
(2..5).each{|i|
  p [i, B(i, i + 2, n)]
}
