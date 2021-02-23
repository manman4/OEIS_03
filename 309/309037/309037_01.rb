def A(n)
  s = 0
  (1..n).each{|i|
    s += 2 ** i * 10 ** (i - 1)
  }
  (1..n - 1).each{|i|
    s += 2 ** i * 10 ** (2 * n - i - 1)
  }
  s
end

n = 500
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
