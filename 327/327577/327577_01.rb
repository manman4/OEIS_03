def T(k, n)
  s = 0
  (0..n - 2).each{|i|
    s += k * n ** i
    return i if s % (i + 1) > 0
  }
  n - 1
end

def A327577(n)
  (1..n - 1).map{|i| T(i, n)}.inject(:+)
end

n = 10000
(2..n).each{|i|
  print i
  print ' '
  puts A327577(i)
}
