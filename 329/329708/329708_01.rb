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
  a = []
  (1..n).each{|i|
    b = (1..i).to_a
    a << mul(b, b, 2 * i - 2)
  }
  a
end

m = 110
n = 9999
ary = A(m).flatten
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}