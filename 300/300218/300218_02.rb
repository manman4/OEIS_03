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

def A300218(n)
  s_ary = []
  ary = [1]
  (1.. n).each{|i|
    b_ary = Array.new(2 * (2 * i - 1) + 1, 0)
    b_ary[0], b_ary[-1] = 1, 1
    ary = mul(ary, b_ary, 2 * i * i)
    s = 0
    0.step(ary.size - 1, i){|j|
      s += ary[j]
    }
    s_ary << s / 2
  }
  s_ary
end

n = 200
ary = A300218(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
