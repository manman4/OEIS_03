def A(n)
  a = [1]
  ary = [0, 1]
  (1..n - 1).each{|i|
    a << 0
    b = [0]
    (0..i - 1).each{|j|
        b[j + 1] = a[j] + (i - 1) * a[j + 1]
    }
    a = b
    ary << (1..i).inject(0){|s, j| s + (-1) ** (i - j) * a[j] * ary[j]}
  }
  ary
end

n = 500
ary = A(n)
(1..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

