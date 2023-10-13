require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

def A(k, n)
  s = 0
  (1..n).each{|i|
  s += A008683(i) * A008683(n / i) * i ** k if n % i == 0
  }
  s
end

def A347227(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << A(i - j, j)
    }
  }
  ary
end

n = 140
ary = A347227(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
