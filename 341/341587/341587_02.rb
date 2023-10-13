def A(n)
  a = [1]
  ary = [[1]]
  (1..n).each{|i|
    a << 0
    b = [0]
    (0..i - 1).each{|j|
      b[j + 1] = a[j] + (i - 1) * a[j + 1]
    }
    a = b
    ary << b.clone # ary << bではなく、ary << b.cloneを書くことで、a, bを変更しても、aryには影響を与えない
  }
  ary
end

n = 500
ary = A(n)
(2..n).each{|i|
  j = (2..i).inject(0){|s, j| s + ary[i][j] * ary[j][2]}
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
