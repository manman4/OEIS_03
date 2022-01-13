def A(n)
  ary = [[1]]
  (1..n).each{|i|
    a = [0] * (i + 1)
    (1..i).each{|j|
      a[j] += ary[i - j][j]     if i - j >= j
      k = i - 2 * j + 1
      a[j] += ary[k][j - 1]     if k     >= 0 && k     >= j - 1
      a[j] += ary[k - j][j - 1] if k - j >= 0 && k - j >= j - 1
    }
    ary << a
  }
  ary
end

def B(n)
  ary = A(n)
  (0..n).map{|i| (0..i).inject(0){|s, j| s + (-1) ** j * ary[i][j]}}
end

n = 10100
m = 10000
ary = B(n)
(0..m).each{|i|
  print i
  print ' '
  puts ary[i]
}

