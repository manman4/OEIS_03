def A(k, n)
  s = 0
  (1..n).each{|i|
    s += (-1) ** (n / i + 1) * i ** k if n % i == 0
  }
  s
end

n = 100
b=[]
(1..n).each{|i|
  j = A(9, i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
b<<j
}
p b
