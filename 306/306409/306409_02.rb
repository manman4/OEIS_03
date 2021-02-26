def A120305(n)
  a, b, c = 1, 1, 3
  cnt = 0
  ary = [1]
  while cnt < n
    cnt += 1
    a, b, c = b, c, (3 * (9 * (cnt + 2) ** 2 - 19 * (cnt + 2) + 8) * c - 3 * (cnt + 1) * (3 * cnt + 2) * b - 2 * (2 * cnt + 1) * (3 * cnt + 4) * a) / (2 * (cnt + 2) * (3 * cnt + 1))
    ary << a
  end
  ary
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A306409(n)
  a = A120305(n)
  s = 0
  ary = []
  (0..n).each{|i|
    s += ncr(2 * i, i)
    ary << (s - a[i]) / 2
  }
  ary
end

n = 2000
ary = A306409(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
