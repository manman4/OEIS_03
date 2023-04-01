def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    ary << (2..i).inject(0){|s, j| s + j * ary[i / j]}
  }
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
