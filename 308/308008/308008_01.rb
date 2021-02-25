def ncr(n, r)
  return 0 if n < r
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  ary = [1, 1]
  (2..n).each{|i|
    ary << 1 + ary[-1] + (1..i - 1).inject(0){|s, j| s + ncr(2 * i - 2, 2 * j - 1) * ary[-j]}
  }
  ary[0..n]
end

n = 300
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
