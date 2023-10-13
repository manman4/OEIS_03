def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

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
  m = f(n)
  c_ary = [1]
  # n! * C(x,0) = n!
  ary = [m] + [0] * n
  (1..n).each{|i|
    c_ary = mul(c_ary, [-i + 1, 1], i)
    m_i = m / f(i)
    (0..i).each{|j| ary[j] += m_i * c_ary[j]}
    ary
  }
  ary
end

n = 500
(0..n).each{|i|
  j = A(2 * i)[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
