# (n+1) * a(n) = 2 * (7*n-3) * a(n-1) - 13 * (n-1) * a(n-2) for n > 1.

def A337167(n)
  a, b = 1, 4
  cnt = 1
  ary = [1, 4]
  while cnt < n
    cnt += 1
    a, b = b, (2 * (7 * cnt - 3) * b - 13 * (cnt - 1) * a) / (cnt + 1)
    ary << b
  end
  ary
end

n = 1000
ary = A337167(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

