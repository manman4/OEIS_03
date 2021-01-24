def A(n)
  s = 0
  (1..n).each{|i|
    if n % i == 0
      s += (n / i) ** i if n / i % 2 == 1
    end
  }
  s
end

n = 5000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}