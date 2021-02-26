def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def I(ary, n)
  a = [1]
  i = 0
  while i < n
    a << -(0..i).inject(0){|s, j| s + ary[1 + i - j] * a[j]}
    i += 1
  end
  a
end

def A(k, n)
  a = (0..k).map{|i| (-1) ** (i % 2) * ncr(k, i)}
  a[k] += 1
  if n > k
    a += [0] * (n - k)
  else
    a = a[0..n]
  end
  I(a, n)
end

def B(k, n)
  A(k + 1, n)
end

def A306846(n)
  a = []
  (0..n).each{|i| a << B(i, n - i)}
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 139
ary = A306846(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
