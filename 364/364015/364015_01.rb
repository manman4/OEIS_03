def A(n)
  s = 0
  (1..n).each{|i|
    if n % i == 0
      s += (-1) ** (n / i) * i if (n / i) % 3 == 2
    end
  }
  s
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}
