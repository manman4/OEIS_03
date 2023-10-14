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

def f(n)
  return 1 if n < 2
  return (1..n).inject(:*)
end

def A(ary, n)
  a = [1]
  ary1 = [0]
  (1..n).each{|i|
    a << 0
    b = [0]
    (0..i - 1).each{|j|
      b[j + 1] = a[j] + (i - 1) * a[j + 1]
    }
    a = b
    ary1 << (0..i).inject(0){|s, j| s + ary[j] * a[j]}
  }
  ary1
end

def B(k, m, n)
  ary = [0] + (1..n).map{|i| f(i - 1)}
  (k - 1).times{ary = A(ary, n)}
  a = (0..n).map{|i| ary[i] / f(i).to_r}
  b = [1] + Array.new(n, 0)
  m.times{b = mul(b, a, n)}
  (0..n).map{|j| (b[j] * f(j) / f(m)).to_i}
end

n = 22
ary = B(2, 2, n)
(2..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
