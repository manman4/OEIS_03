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

def I(ary, n)
  a = [1]
  (0..n - 1).each{|i| a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}}
  a
end

def A(ary, n)
  a = [1] + [0] * n
  a_ary = []
  (1..n).each{|i|
    b_ary = Array.new(n + 1, 0)
    j = ary[i] - a[i]
    a_ary << j
    b_ary[0], b_ary[i] = 1, j
    a = mul(a, I(b_ary, n), n)
  }
  a_ary
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 50
a = (0..n).map{|i| (-1) ** i}
ary = A(a, n).map{|i| -i}
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
