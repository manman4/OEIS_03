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
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

n = 100
nums = (1..n).to_a
ps = Array.new(n + 1){0}
ps[0] = 1
nums.each{|num|
  # ������num�Ȃ���̂��J�E���g
  (num..n).each{|i|
    ps[i] += ps[i - num]
  }
}
ps4 = (0..n).map{|i| ps[i] ** 4}
ps0 = I(ps, n)
ary = mul(ps4, ps0, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
