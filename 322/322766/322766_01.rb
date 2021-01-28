def bell(n)
  i = 1
  bell = [1]
  ary = [1, 1]
  while i < n
    next_bell = [bell[-1]]
    # Bell triangle
    i.times{|j| next_bell[j + 1] = next_bell[j] + bell[j]}
    bell = next_bell
    ary << bell[-1]
    i += 1
  end
  ary
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  k = 0
  a = [bell(2 * n)]
  while k < n
    k += 1
    a << (0..2 * (n - k)).map{|i| (0..k - 1).inject(a[k - 1][i + 1] + a[k - 1][i + 2]){|s, j| s + ncr(k - 1, j) * a[j][i]} / 2}
  end
  a
end

n = 300
ary = A(n + 1)
(0..n).each{|i|
  j = ary[i][1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}