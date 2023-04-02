def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    ary << -(2..i).inject(0){|s, j| s + j * j * ary[i / j]}
  }
  ary
end

n = 10010
m = 10000
ary = A(n)
(1..m).each{|i|
  print i
  print ' '
  puts ary[i]
}
