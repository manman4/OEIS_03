def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  a = [1]
  (1..n).each{|i|
    a << i * (0..i - 1).inject(0){|s, j| s + k ** (i - 1 - j) * ncr(i - 1, j) * a[j]}
  }
  a
end

n = 500
ary = A(4, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

