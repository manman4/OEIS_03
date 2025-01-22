def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n, m)
  ary = [0] * (m + 1)
  (m + 1..n).each{|i|
    ary << (m + 1..i - 1).inject(ncr(i, m)){|s, j| s + ncr(j, m) * ncr(i - 1, j) * ary[i - j]}
  }
  ary
end

n = 500
ary = A(n, 3)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

