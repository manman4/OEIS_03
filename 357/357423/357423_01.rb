def A(k, n)
  # aは符号なし
  a = [1]
  ary = [0]
  (1..n).each{|i|
    a << 0
    b = [0]
    (0..i - 1).each{|j|
      b[j + 1] = a[j] + (i - 1) * a[j + 1]
    }
    a = b
    # i>0 のときa[0]=0
    ary << (1..i).inject(0){|s, j| s + (i + k * j) ** (j - 1) * (-1) ** (i - j) * a[j]}
  }
  ary
end

n = 700
ary = A(-1, n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
