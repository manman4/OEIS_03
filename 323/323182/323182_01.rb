def A(k, n)
  a, b = 1, 2 * k
  ary = [1]
  cnt = 0
  while cnt + 1 <= n
    a, b = b, 2 * k * b - a
    cnt += 1
    ary << a
  end
  ary
end

def A322836(n)
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
ary = A322836(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
