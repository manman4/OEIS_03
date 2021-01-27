def A(k, n)
  a, b = [1], [0, 1]
  ary = [1]
  cnt = 0
  while cnt + 1 <= n
    c = ([0] + b).map{|i| i * 2}
    (0..cnt).map{|i| c[i] -= a[i]}
    a, b = b, c
    cnt += 1
    ary << (0..cnt).inject(0){|s, i| s + k ** i * a[i]}
  end
  ary
end

def B(n)
  return 1 if n == 0
  A(n - 1, n)[-1]
end

n = 500
(0..n).each{|i|
  j = B(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
