def A(k, n)
  ary = [0, 1]
  (2..n).each{|i|
    ary << (2..i).inject(0){|s, j| s + (-1) ** j * j ** k * ary[i / j]}
  }
  ary
end

n = 10010
m = 10000
ary = A(2, n)
(1..m).each{|i|
  print i
  print ' '
  puts ary[i] - ary[i - 1]
}
