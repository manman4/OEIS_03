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
  a = (0..n).map{|i| f(i)}
  b = (0..n).map{|i| (-1) ** (i % 2) * a[i]}
  mul(a, I(b, n), n)
end

n = 50
ary = A(n)
(0..n).each{|i|
  j = ary[i].to_i
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
