def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# k+1に注意
def A(k, n)
  ary = [1]
  (1..n).each{|i|
    ary << (f(i - 1) * (1..[i, k + 1].min).inject(0){|s, j| s - ary[i - j] / f(i - j).to_r}).to_i
  }
  ary
end
def A293669(n)
  a = []
  (0..n).each{|i| a << A(i, n - i)}
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end
n = 139
ary = A293669(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}