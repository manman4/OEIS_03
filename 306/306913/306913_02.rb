def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (-1) ** (n % 2) * (0..n / k).inject(0){|s, i| s + (-1) ** (((k + 1) % 2 * i) % 2) * ncr(n + k - 1, k * i + k - 1)}
end

def B(k, n)
  (0..n).map{|i| A(k + 1, i)}
end

def A306913(n)
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
ary = A306913(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
