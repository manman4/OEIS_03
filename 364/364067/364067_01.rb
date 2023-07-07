def A(n, mod, k)
  s = 0
  (1..n).each{|i|
    if n % i == 0
      s += i + mod - k if i % mod == k
    end
  }
  s / mod
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(3 * i - 2, 3, 2)
}

