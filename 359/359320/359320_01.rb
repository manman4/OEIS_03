def A(n)
  a = [1]
  ary = [1]
  (1..n).each{|i|
    b = a + [0] * (i ** 4)
    (0..a.size - 1).each{|j| b[i ** 4 + j] += a[j]}
    a = b
    ary << a.max
  }
  ary
end

n = 50
p ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
