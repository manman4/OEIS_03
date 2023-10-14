def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  ary = [1] + [0] * (k - 1)
  (k..n).each{|i|
    ary << (0..i - k).inject(0){|s, j| s - ncr(i - k, j) * ary[j]}
  }
  ary
end

n = 700
ary = A(2, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
