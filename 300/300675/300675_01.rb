require 'prime'

def A008683(n)
  ary = n.prime_division
  return (-1) ** (ary.size % 2) if ary.all?{|i| i[1] == 1}
  0
end

def A(k, n)
  s = 0
  (1..n).each{|i|
    s += A008683(i) * k ** (n / i) if n % i == 0 && i % 2 == 1
  }
  s / (k * n)
end

n = 500
(1..n).each{|i|
  j = A(16, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
