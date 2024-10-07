def a(n)
  cnt = 0
  (0..n - 1).each{|i|
    (i..n - 1).each{|j|
      cnt += 1 if (i * i + i * j + j * j) % n == 0
    }
  }
  cnt
end

n = 10000
(1..n).each{|i|
  j = a(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
