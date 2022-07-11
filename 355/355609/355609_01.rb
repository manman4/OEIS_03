def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, m, n)
  ary = [1]
  x = f(m)
  (1..n).each{|i|
    y = f(i - 1) / x
    ary << (k + 1..i).inject(0){|s, j| s + ary[-j] * j * y / (f(i - j) * (j - k).to_r)}.to_i
  }
  ary
end

def A355609(n)
  a = []
  (0..n).each{|i| a << A(i, 1, n - i)}
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 139
ary = A355609(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}