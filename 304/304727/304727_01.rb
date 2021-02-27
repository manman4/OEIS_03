def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  a, b = 0, 1
  ary = [0]
  cnt = 0
  while cnt < n
    a, b = b, b * f(cnt + 2) + 1 * a
    ary << a
    cnt += 1
  end
  ary
end

n = 100
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
