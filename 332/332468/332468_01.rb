require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

def A(n)
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  ary = [0] + (1..n).map{|i| (1..i).inject(0){|s, j| s + m_ary[j] * (i / j) ** i}}
end

n = 1000
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
