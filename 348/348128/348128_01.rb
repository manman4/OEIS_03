require 'prime'

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

def A(ary, n)
  a = [1] + [0] * n
  a_ary = []
  (1..n).each{|i|
    b_ary = Array.new(n + 1, 0)
    j = ary[i] - a[i]
    a_ary << j
    b_ary[0], b_ary[i] = 1, j
    a = mul(a, b_ary, n)
  }
  a_ary
end

n = 1010
m = 1000
a = []
(0..n).each{|i|
  if i.prime?
    a << 1
  else
    a << 0
  end
}
ary = A(a, n)
(1..m).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
