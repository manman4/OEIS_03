def A(n)
  ary = [0, 1]
  (2..n).each{|i|
    ary << (2..i).inject(0){|s, j| s + (-1) ** j * j * ary[i / j]}
  }
  ary
end

n = 10010
m = 2 ** 13 - 1
ary = A(n)
(1..m).each{|i|
  print i
  print ' '
  puts ary[i]
}
