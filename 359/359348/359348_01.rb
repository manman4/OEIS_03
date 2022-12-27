def A(n)
  a = [1]
  ary = [1]
  (1..n).each{|i|
    t = i * (i + 1) / 2
    b = a + [0] * t
    (0..a.size - 1).each{|j| b[t + j] += a[j]}
    a = b
    ary << a.max
  }
  ary
end

n = 210
m = 200
ary = A(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
