def S(s, t, n)
  a, b = 1, s
  ary = [1]
  cnt = 0
  while cnt < n
    a, b = b, s * b * (cnt + 2) + t * a
    ary << a
    cnt += 1
  end
  ary
end

def A(k, n)
  S(k, 1, n)
end

n = 500
(0..n).each{|i|
  j = A(i, i)[-1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
