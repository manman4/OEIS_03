def A(k, n)
  s = 0
  (1..n).each{|i|
    s += i if n % i == 0 && i % k != 0
  }
  s
end

def B(k, n)
  (1..n).map{|i| A(k, i)}
end

def C(k, n)
  ary = [1]
  a = [0] + B(k, n)
  (1..n).each{|i| ary << (1..i).inject(0){|s, j| s + a[j] * ary[-j]} / i}
  ary
end

def D(n)
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

def E(n)
  ary = D(n)
  (0..n).map{|i| ary[i].inject(:+)}
end

n = 10100
m = 10000
ary0 = C(3, n)
ary1 = E(n)
(0..m).each{|i|
  print i
  print ' '
  puts ary0[i] - ary1[i]
}
