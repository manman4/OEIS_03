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

# [1, f, f^2, f^3, ...]
def A(f_ary, n)
  g_ary = [1] + [0] * n
  a = [g_ary]
  n.times{
    g_ary = mul(f_ary, g_ary, n)
    a << g_ary
  }
  a
end

# f_aryの1次の項は1であること
def f5r(f_ary, n)
  a = A(f_ary, n)
  b = []
  (0..n).each{|i|
    c = [0] * (n + 1)
    c[i] = 1
    b << c
  }
  (2..n).each{|i1|
    # 計算の順に注意
    (i1 - 1).downto(1){|x|
      b[x][i1] = (a[x][i1] - (x + 1..i1 - 1).inject(0){|s, i2|
        s + (i2..i1).inject(b[i2][i1]){|t, i3|
          t + (i3..i1).inject(b[i3][i1]){|u, i4|
            u + (i4..i1).inject(b[i4][i1]){|v, i5|
              v + b[i5][i1] * b[i4][i5]
            } * b[i3][i4]
          } * b[i2][i3]
        } * b[x][i2]
      }) / 5r
    }
  }
  b[1]
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 17
m = 17 
f_ary = [0] + (1..n).map{|i| 5 ** (i - 1) / f(i - 1).to_r}
ary = f5r(f_ary, n)
a = [0] + (1..n).map{|i| f(i) * ary[i]}
p [0] + (1..n).map{|i| (f(i) * ary[i]).numerator}
(0..m).each{|i|
  j = a[i].numerator
  break if a[i].denominator > 1
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}