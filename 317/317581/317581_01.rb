require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

def A(n)
  ary = [0]
  (1..n).each{|i|
    s = 1
    (1..i - 1).each{|j|
      s += A008683(i / j) * ary[j] if i % j == 0
    }
    ary << s
  }
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
