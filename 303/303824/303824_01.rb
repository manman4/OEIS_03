def A(k, n)
  ary = [1]
  (1..n).each{|i|
    s = ary[i / k]
    s += ary[i / k - 1] if i % k == 0
    ary << s
  }
  ary
end

n = 10000
ary = A(6, n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
