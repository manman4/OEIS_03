def A(k, m, n)
  a, b = k, m
  ary = [k]
  (1..n).each{|i|
    a, b = b, (2 * i + 1) * b + i ** 2 * a
    ary << a
  }
  ary
end

def B(n)
  # A054765
  ary0 = A(0, 1, n)
  # A012244
  ary1 = A(1, 1, n)
  (0..n).map{|i| 4 * ary0[i] / ary1[i].to_r}
end

n = 1000
ary = B(n)
(0..n).each{|i|
  j = ary[i].denominator
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
