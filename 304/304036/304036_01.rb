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

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
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

def A(n)
  ary = [1]
  s = 0
  (1..n).each{|i|
    ik = f(i)
    break if ik > n
    s += ik
    b_ary = Array.new(ik + 1, 0)
    b_ary[0], b_ary[-1] = 1, -1
    ary = mul(ary, b_ary, s)
  }
  a = ary.size - 1
  return ary + [0] * (n - a) if n > a
  ary[0..n]
end

def B(n)
  ary = [1]
  s = 0
  (1..n).each{|i|
    ik = f(i) * (i + 2)
    break if ik > n
    s += ik
    b_ary = Array.new(ik + 1, 0)
    b_ary[0], b_ary[-1] = 1, -1
    ary = mul(ary, b_ary, s)
  }
  a = ary.size - 1
  return ary + [0] * (n - a) if n > a
  ary[0..n]
end

n = 50000
ary = mul(I(A(n), n), B(n), n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
