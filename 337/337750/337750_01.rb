def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  ary = [1]
  a, b, c = 1, 1, 1
  cnt = 0
  while cnt < n
    cnt += 1
    a, b, c = b, c, 1 - (cnt + 2) * (cnt + 1) * cnt * a
    ary << a
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