def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  ary = [1]
  (1..n).each{|i|
    ary << 1 + k * (0..i - 1).inject(0){|s, j| s + ncr(i, j) * ary[j] * ary[-1 - j]}
  }
  ary
end

n = 400
ary = A(4, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
