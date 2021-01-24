def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + ncr((k + 1) * n, i) * ncr(k * n - i - 1, n - i)}
end

def A330843(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << A(i - j, j)
    }
  }
  ary
end

n = 139
ary = A330843(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
