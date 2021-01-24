def A(n)
  s = 1
  while (3 ** s + s) % n > 0
    s += 1
  end
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