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

def A(n)
  ary = [1]
  (0..n / 2).each{|i|
    b_ary = Array.new(2 * i + 2, 0)
    b_ary[0], b_ary[-1] = 1, (-1) ** (i % 2)
    ary = mul(ary, b_ary, n)
  }
  ary
end

m = 1100
ary = A(m)
(0..1000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
