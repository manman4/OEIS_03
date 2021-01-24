def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  ary = [1, 2 * k]
  (2..n).each{|i|
    ary << (i + 1) * (k * (2 * i + 1) * ary[-1] - (k - 2) ** 2 * i * ary[-2]) / (i * (i + 2))
  }
  ary
end

n = 1000
ary = A(4, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
