def A(n)
  s = 0
  (1..n - 1).each{|i|
    j = i + n
    s += 1 if Math.sqrt(j).to_i ** 2 == j
  }
  s
end

n = 10000
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
