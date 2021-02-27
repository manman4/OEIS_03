def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  a, b = 1, 40
  ary = [1]
  i = 0
  while i < n
    i += 1
    a, b = b, (8 * (2 * i + 1) * (8 * i * i + 8 * i + 5) * b - 4096 * i ** 3 * a) / (i + 1) ** 3
    ary << a
  end
  ary
end

n = 1000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
