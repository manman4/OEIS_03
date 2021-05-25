require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

def A(n)
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  ary = [0, 1]
  (2..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += m_ary[i / j] * (j - 1) * j if i % j == 0
    }
    ary << 3 * s
  }
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  print i
  print ' '
  puts j
}
