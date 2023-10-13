def A(n)
  a = [1]
  s = 1
  t = 0
  h_ary = [0]
  ary = [0]
  (1..n).each{|i|
    a << 0
    s *= i
    t *= i
    t += s / i
    h_ary << t
    b = [0]
    (0..i - 1).each{|j|
      b[j + 1] = a[j] + (i - 1) * a[j + 1]
    }
    a = b
    ary << (1..i).inject(0){|s, j| s + h_ary[j] * b[j]}
  }
  ary
end

n = 500
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
