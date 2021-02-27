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
  S(k, -1, n)
end

def A305466(n)
  a = []
  (0..n).each{|i| a << A(i, n - i)}
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 139
ary = A305466(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
