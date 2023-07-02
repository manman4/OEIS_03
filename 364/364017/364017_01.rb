def A(n)
  s = 0
  (1..n).each{|i|
    if n % i == 0
      s += (-1) ** (n / i) * i * (i + 1) / 2 if (n / i) % 3 == 1
    end
  }
  -s
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}
