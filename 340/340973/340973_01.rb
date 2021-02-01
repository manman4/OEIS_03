def A340973(n)
  a, b = 1, 7
  cnt = 1
  ary = [1, 7]
  while cnt < n
    cnt += 1
    a, b = b, (7 * (2 * cnt - 1) * b - 13 * (cnt - 1) * a) / cnt
    ary << b
  end
  ary
end

n = 1000
ary = A340973(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

