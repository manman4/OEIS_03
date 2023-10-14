require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

# a[0] = 0
def A(k, n)
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  ary = [0]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += j ** k * m_ary[j] ** 2 if i % j == 0
    }
    ary << s
  }
  ary
end

n = 10000
ary = A(9, n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
