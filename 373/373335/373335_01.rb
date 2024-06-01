# 約数で(1 mod k) - (2 mod k)
def A(k, n)
  s = 0
  (1..n).each{|i|
    if n % i == 0
      s += 1 if i % k == 1
      s -= 1 if i % k == 2
    end
  }
  s
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(5, i)
}
