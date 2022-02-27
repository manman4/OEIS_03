require 'prime'

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end


def A(n)
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  a = [1]
  (1..n).each{|i|
    a << (1..i).inject(0){|s, j| s + m_ary[j] * ncr(i - 1, j - 1) * a[-j]}
  }
  a
end

n = 600
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}