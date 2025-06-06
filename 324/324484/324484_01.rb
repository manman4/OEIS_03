require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

# a[0] = 0
def Möbius_transform(ary, n)
  m_ary = [0] + (1..n).map{|i| A008683(i)}
  a = [0]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += m_ary[i / j] * ary[j] if i % j == 0
    }
    a << s
  }
  a
end

def A001350(n)
  a, b = 0, 1
  cnt = 0
  ary = [0]
  while ary.size <= n
    cnt += 1
    a, b = b, a + b + 1 + (-1) ** (cnt % 2)
    ary << a
  end
  ary
end

def A324484(n)
  ary = A001350(n).map{|i| i * i}
  Möbius_transform(ary, n)
end

m = 3000
ary = A324484(m)
(1..2000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}


