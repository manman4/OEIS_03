# 予想

def A(n)
  ary = []
  a, b, c, d, e, f = 1, 10, 101, 1105, 12046, 131399 #, 1433341
  while ary.size < n + 1
    ary << a
    a, b, c, d, e, f = b, c, d, e, f, 10 * f + 10 * e - 10 * c - 10 * b + a
  end
  ary
end

n = 500
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}