def A(k, n)
  s = 0
  (1..n).each{|i|
    j = i * i
    break if j > n
    s += (n / i.to_r) ** k if n % j == 0
  }
  s
end

n = 10000
(1..n).each{|i|
  j = A(3, i)
  break if j.denominator > 1
  print i
  print ' '
  puts j.to_i
}

