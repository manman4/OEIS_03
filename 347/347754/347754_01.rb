def A347754(n)
  s = 1
  ary = []
  (1..n).each{|i|
    j = i * i + s * s
    k = Math.sqrt(j).floor + 1
    ary << k
    s = k * k - j
  }
  ary
end

n = 10000
ary = A347754(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
