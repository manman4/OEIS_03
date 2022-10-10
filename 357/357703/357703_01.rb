def A(k, n)
  ary = [1, 0]
  a, b = 1, 0
  (2..n).each{|i|
    c = (2 * i - 3) * b - (i * i - 4 * i + 4 - k) * a
    ary << c
    a, b = b, c
  }
  ary
end

n = 1000
ary = A(3, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}